#include <CSpellBuild.h>
#include <CSpellI.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <unistd.h>

/*------------------------------------------------------------------*
 *
 * Build Dictionary Hash File from list of words.
 *
 *   CSpellBuild [<dictionary_file>] [<hash_file>]
 *
 *     <dictionary_file> : Optional Dictionary Filename
 *     <hash_file>       : Optional Hash Filename
 *
 *------------------------------------------------------------------*/

int
main(int argc, char **argv)
{
  int   i;
  FILE *statf;
  FILE *countf;
  int   stats[NSTAT];

  /* Extract Dictionary File Name and Hash File Name from
     Command Line Arguments (if specified), otherwise use
     default Names */

  if (argc > 1) {
    argv++;

    dictionary_file = *argv;

    if (argc > 2) {
      argv++;

      hash_file = *argv;
    }
    else
      hash_file = (char *) DEFHASH;
  }
  else {
    dictionary_file = (char *) DEFDICT;
    hash_file       = (char *) DEFHASH;
  }

  /* Set up Names of Count and Stat File for Dictionary */

  sprintf(dictionary_count_file, "%s.cnt" , dictionary_file);
  sprintf(dictionary_stat_file , "%s.stat", dictionary_file);

  /* Check if Dictionary File Exists and is Readable */

  if (access(dictionary_file, 2) < 0) {
    fprintf(stderr, "No dictionary (%s)\n", dictionary_file);
    exit(1);
  }

  /* If Dictionary Count File does not Exist Create a new one */

  if (access(dictionary_count_file, 2) < 0)
    CSpellCreateNewCountFile();

  /* Open Count File and Get Number of Words in Dictionary */

  if ((countf = fopen(dictionary_count_file, "r")) == nullptr) {
    fprintf(stderr, "No count file\n");
    exit(1);
  }

  numwords = 0;

  fscanf(countf, "%d", &numwords);

  fclose(countf);

  /* Ensure Count File not Corrupt */

  if (numwords == 0) {
    fprintf(stderr, "Bad count file\n");
    exit(1);
  }

  /* Initialise Hash Table Size form Number of Words */

  cspell_hashsize = numwords;

  /* Read Dictionary Words into Hash Table */

  CSpellReadDictionaryFile();

  /* Open Status File and Write Details of Hash Table Usage to it */

  if ((statf = fopen(dictionary_stat_file, "w")) != nullptr) {
    for (i = 0; i < NSTAT; i++)
      stats[i] = 0;

    for (i = 0; i < cspell_hashsize; i++) {
      if (cspell_hashtbl[i].used == 0) {
        stats[0]++;
      }
      else {
        int j = 1;

        for (CDSpellDEnt *dp = &cspell_hashtbl[i]; dp->next != nullptr; dp = dp->next)
          j++;

        if (j >= NSTAT)
          j = NSTAT - 1;

        stats[j]++;
      }
    }

    for (i = 0; i < NSTAT; i++)
      fprintf(statf, "%d: %d\n", i, stats[i]);

    fclose(statf);
  }
  else
    fprintf(stderr, "Can't create %s\n", dictionary_stat_file);

  CSpellFillHashTable();

  /* Write Hash File */

  CSpellWriteHashFile();

  exit(0);
}

/*------------------------------------------------------------------*
 *
 * CSpellCreateNewCountFile
 *   Create new count file which contains the number
 *   of words in the dictionary file.
 *
 *------------------------------------------------------------------*/

static void
CSpellCreateNewCountFile()
{
  int   i;
  FILE *d;
  char *cp;
  char  buf[200];
  char  lastbuf[200];

  fprintf(stderr, "Counting words in dictionary ...\n");

  /* Open Dictionary File */

  if ((d = fopen(dictionary_file, "r")) == nullptr) {
    fprintf (stderr, "Can't open dictionary\n");
    exit(1);
  }

  for (i = 0, lastbuf[0] = '\0'; fgets(buf, sizeof(buf), d); ) {
    /* Convert to Upper Case */

    for (cp = buf; *cp; cp++) {
      if (islower(*cp))
        *cp = toupper (*cp);
    }

    if (strcmp(buf, lastbuf) != 0) {
      /* Increment Word Count and Print Every 1024 Words */

      if ((++i & 1023) == 0)
        {
          printf ("%d ", i);
          fflush (stdout);
        }

      strcpy(lastbuf, buf);
    }
  }

  fclose(d);

  /* Print Number of Words */

  printf("\n%d words\n", i);

  /* Open Dictionary Count File */

  if ((d = fopen(dictionary_count_file, "w")) == nullptr) {
    fprintf(stderr, "can't create %s\n", dictionary_count_file);
    exit(1);
  }

  /* Write Number of Words to Dictionary Count File */

  fprintf(d, "%d\n", i);

  /* Close Dictionary Count File */

  fclose(d);
}

/*------------------------------------------------------------------*
 *
 * CSpellReadDictionaryFile
 *   Read Words in Dictionary File into internal storage.
 *
 *------------------------------------------------------------------*/

static void
CSpellReadDictionaryFile()
{
  int          i;
  int          h;
  CDSpellDEnt  d;
  char        *p;
  CDSpellDEnt *dp;
  int          len;
  FILE        *dictf;
  char         lbuf[100];

  /* Open Dictionary File */

  if ((dictf = fopen(dictionary_file, "r")) == nullptr) {
    fprintf(stderr, "Can't open dictionary\n");
    exit(1);
  }

  /* Allocate Space for Required Number of Words */

  cspell_hashtbl = new CDSpellDEnt [numwords];

  if (! cspell_hashtbl) {
    fprintf(stderr, "couldn't allocate hash table\n");
    exit(1);
  }

  /* Read Words */

  i = 0;

  while (fgets(lbuf, sizeof(lbuf), dictf) != nullptr) {
    /* Print Message Every 1024 Words */

    if ((i & 1023) == 0) {
      printf ("%d ", i);
      fflush (stdout);
    }

    /* Increment Number of Words */

    i++;

    /* Remove Trailing Newline */

    p = &lbuf[strlen(lbuf) - 1];

    if (*p == '\n')
      *p = 0;

    /* Create Dictionary Entry for Word */

    if (CSpellMakeDictionaryEntry(lbuf, &d) < 0)
      continue;

    len = strlen(lbuf);

    /* Allocate Space for Word (and Capitalization if required) */

#ifdef CSPELL_CAPS
    if (d.followcase)
      d.word = (char *) malloc(2*len + 4);
    else
      d.word = (char *) malloc(len + 1);
#else
    d.word = (char *) malloc(len + 1);
#endif

    if (d.word == nullptr) {
      fprintf(stderr, "couldn't allocate space for word %s\n", lbuf);
      exit(1);
    }

    strcpy(d.word, lbuf);

#ifdef CSPELL_CAPS
    if (d.followcase) {
      p = d.word + len + 1;

      *p++ = 1;     /* Count of capitalizations */
      *p++ = '-';   /* Don't keep in pers dict */

      strcpy(p, lbuf);
    }

    /* Capitalize Word */

    for (p = d.word; *p; p++) {
      if (islower(*p))
        *p = toupper(*p);
    }
#endif

    /* Calculate Hash for Word */

    h = CSpellHash(d.word, len, cspell_hashsize);

    /* Get Dictionary Entry for Hash */

    dp = &cspell_hashtbl[h];

    /* If Dictionary Entry not Used then Set Directly */

    if (dp->used == 0) {
      *dp = d;
    }
    else {
#ifdef CSPELL_CAPS
      while (dp != nullptr && strcmp (dp->word, d.word) != 0)
        dp = dp->next;

      if (dp != nullptr) {
        if (d.followcase || (dp->followcase && !d.allcaps && !d.capitalize)) {
          /* Add a specific capitalization */

          if (dp->followcase) {
            p = &dp->word[len + 1];

            (*p)++; /* Bump counter */

            dp->word = (char *) realloc(dp->word, ((*p & 0xFF) + 1)*(len + 2));

            if (dp->word == nullptr) {
              fprintf(stderr, "couldn't allocate space for word %s\n", lbuf);
              exit(1);
            }

            p = &dp->word[len + 1];

            p += ((*p & 0xFF) - 1)*(len + 2) + 1;

            *p++ = '-';

            strcpy(p, d.followcase ? &d.word[len + 3] : lbuf);
          }
          else {
            /* d.followcase must be true */
            /* thus, d.capitalize and d.allcaps are */
            /* clear */

            free(dp->word);

            dp->word         = d.word;
            dp->followcase   = 1;
            dp->k_followcase = 1;

            /* Code later will clear dp->allcaps. */
          }
        }

        /* Combine two capitalizations.  If d was */
        /* allcaps, dp remains unchanged */

        if (d.allcaps == 0) {
          /* dp is the entry that will be kept.  If */
          /* dp is followcase, the capitalize flag */
          /* reflects whether capitalization "may" */
          /* occur.  If not, it reflects whether it */
          /* "must" occur. */

          if (d.capitalize) {  /* i.e. lbuf was cap'd */
            if (dp->followcase)
              dp->capitalize = 1; /* May */
            else if (dp->allcaps) /* i.e. not lcase */
              dp->capitalize = 1; /* Must */
          }
          else { /* lbuf was followc or all-lc */
            if (!dp->followcase)
              dp->capitalize = 0;    /* May */
          }

          dp->k_capitalize = dp->capitalize;
          dp->allcaps      = 0;
          dp->k_allcaps    = 0;
        }
      }
      else {
#endif
        dp = (CDSpellDEnt *) malloc(sizeof(CDSpellDEnt));

        if (dp == nullptr) {
          fprintf(stderr, "couldn't allocate space for collision\n");
          exit(1);
        }

        *dp                    = d;
        dp->next               = cspell_hashtbl[h].next;
        cspell_hashtbl[h].next = dp;
      }
    }
#ifdef CSPELL_CAPS
  }
#endif

  printf("\n");
}

/*------------------------------------------------------------------*
 *
 * CSpellMakeDictionaryEntry
 *   Fill in the flags in the Dictionary Entry Structure
 *   and Put a Null after the word.
 *
 *     int flag = CSpellMakeDictionaryEntry(char *lbuf, CDSpellDEnt *d);
 *
 *       lbuf : Word whose Entry is to be processed.
 *       d    : Dictionary Entry Structure.
 *       flag : Whether the Dictionary Entry was successfully processed.
 *            :   0 - OK, -1 - Failed.
 *
 *------------------------------------------------------------------*/

static int
CSpellMakeDictionaryEntry(char *lbuf, CDSpellDEnt *d)
{
  char *p;

  d->next       = nullptr;
  d->used       = 1;
  d->v_flag     = 0;
  d->n_flag     = 0;
  d->x_flag     = 0;
  d->h_flag     = 0;
  d->y_flag     = 0;
  d->g_flag     = 0;
  d->j_flag     = 0;
  d->d_flag     = 0;
  d->t_flag     = 0;
  d->r_flag     = 0;
  d->z_flag     = 0;
  d->s_flag     = 0;
  d->p_flag     = 0;
  d->m_flag     = 0;
  d->keep       = 0;

#ifdef CSPELL_CAPS
  d->allcaps    = 0;
  d->capitalize = 0;
  d->followcase = 0;

  /*
  ** Figure out the capitalization rules from the capitalization of
  ** the sample entry.  Only one of followcase, allcaps, and capitalize
  ** will be set.  Combinations are generated by higher-level code.
  */

  for (p = lbuf; *p && *p != '/'; p++) {
    if (islower(*p))
      break;
  }

  if (*p == '\0' || *p == '/')
    d->allcaps = 1;
  else {
    for (; *p && *p != '/'; p++) {
      if(isupper(*p))
        break;
    }

    if (*p == '\0' || *p == '/') {
      /*
      ** No uppercase letters follow the lowercase ones.
      ** If the first two letters are capitalized, it's
      ** "followcase". If the first one is capitalized, it's
      ** "capitalize".
      */

      if (isupper(lbuf[0])) {
        if (isupper(lbuf[1]))
          d->followcase = 1;
        else
          d->capitalize = 1;
      }
    }
    else
      d->followcase = 1;  /* .../lower/upper */
  }

  d->k_allcaps    = d->allcaps;
  d->k_capitalize = d->capitalize;
  d->k_followcase = d->followcase;
#endif

  p = strchr(lbuf, '/');

  if (p != nullptr)
    *p = 0;

  if (strlen(lbuf) > CSPELL_WORDLEN - 1) {
    printf("%s: word too big\n", lbuf);
    return(-1);
  }

  if (p == nullptr)
    return(0);

  p++;

  while (*p != '\0' && *p != '\n') {
    if (islower(*p))
      *p = toupper (*p);

    switch (*p) {
      case 'V':
        d->v_flag = 1;
        break;
      case 'N':
        d->n_flag = 1;
        break;
      case 'X':
        d->x_flag = 1;
        break;
      case 'H':
        d->h_flag = 1;
        break;
      case 'Y':
        d->y_flag = 1;
        break;
      case 'G':
        d->g_flag = 1;
        break;
      case 'J':
        d->j_flag = 1;
        break;
      case 'D':
        d->d_flag = 1;
        break;
      case 'T':
        d->t_flag = 1;
        break;
      case 'R':
        d->r_flag = 1;
        break;
      case 'Z':
        d->z_flag = 1;
        break;
      case 'S':
        d->s_flag = 1;
        break;
      case 'P':
        d->p_flag = 1;
        break;
      case 'M':
        d->m_flag = 1;
        break;
      case 0:
        fprintf(stderr, "no flags on word %s\n", lbuf);
        continue;
      default:
        fprintf(stderr, "unknown flag %c word %s\n", *p, lbuf);
        break;
    }

    p++;

    if (*p == '/')  /* Handle old-format dictionaries too */
      p++;
  }

  return 0;
}

/*------------------------------------------------------------------*
 *
 * CSpellFillHashTable
 *   Fill Hash Table with words from Dictionary.
 *
 *------------------------------------------------------------------*/

static void
CSpellFillHashTable()
{
  int          i;
  CDSpellDEnt *dp;
  CDSpellDEnt *nextword;
  CDSpellDEnt *freepointer;

  for (freepointer = cspell_hashtbl; freepointer->used; freepointer++)
    ;

  for (nextword = cspell_hashtbl, i = numwords; i != 0; nextword++, i--) {
    if (nextword->used == 0 || nextword->next == nullptr ||
        (nextword->next >= cspell_hashtbl &&
         nextword->next < cspell_hashtbl + cspell_hashsize))
      continue;

    dp = nextword;

    while (dp->next) {
      if (freepointer > cspell_hashtbl + cspell_hashsize) {
        fprintf(stderr, "table overflow\n");
        getchar();
        break;
      }

      *freepointer = *(dp->next);

      dp->next = freepointer;
      dp       = freepointer;

      while (freepointer->used)
        freepointer++;
    }
  }
}

/*------------------------------------------------------------------*
 *
 * CSpellWriteHashFile
 *   Write the Hash File.
 *
 *------------------------------------------------------------------*/

static void
CSpellWriteHashFile()
{
  int                i;
  int                n;
  long               strptr;
  FILE              *outfile;
  CDSpellHashHeader  hashheader;

  /* Open Hash File for Write */

  if ((outfile = fopen(hash_file, "w")) == nullptr) {
    fprintf(stderr, "can't create %s\n", hash_file);
    return;
  }

  /* Initialise and Write Header Block */

  hashheader.magic      = CSPELL_MAGIC;
  hashheader.stringsize = 0;
  hashheader.tblsize    = cspell_hashsize;

  fwrite((char *) &hashheader, sizeof(hashheader), 1, outfile);

  /* Write Hash Table */

  strptr = 0;

  for (i = 0; i < cspell_hashsize; i++) {
    n = strlen(cspell_hashtbl[i].word) + 1;

#ifdef CSPELL_CAPS
    if (cspell_hashtbl[i].followcase)
      n += (cspell_hashtbl[i].word[n] & 0xFF)*(n + 1) + 1;
#endif

    fwrite(cspell_hashtbl[i].word, n, 1, outfile);

    cspell_hashtbl[i].word = (char *) strptr;

    strptr += n;
  }

  /* Pad file to a CDSpellDEnt boundary for efficiency. */

  n = (strptr + sizeof(hashheader)) % sizeof(CDSpellDEnt);

  if (n != 0) {
    n = sizeof(CDSpellDEnt) - n;

    strptr += n;

    while (--n >= 0)
      putc('\0', outfile);
  }

  /* Write Closing CDSpellDEnt */

  for (i = 0; i < cspell_hashsize; i++) {
    if (cspell_hashtbl[i].next != 0) {
      long x;

      x = cspell_hashtbl[i].next - cspell_hashtbl;

      cspell_hashtbl[i].next = (CDSpellDEnt *) x;
    }
    else
      cspell_hashtbl[i].next = (CDSpellDEnt *) - 1;
  }

  fwrite((char *) cspell_hashtbl, sizeof(CDSpellDEnt), cspell_hashsize, outfile);

  /* Re-write Header Block with Actual String Size */

  hashheader.stringsize = (int) strptr;

  rewind(outfile);

  fwrite((char *) &hashheader, sizeof(hashheader), 1, outfile);

  /* Close Hash File */

  fclose(outfile);
}
