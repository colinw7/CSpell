#ifndef CDOC_SPELL_BUILD_P_H
#define CDOC_SPELL_BUILD_P_H

/*----------------------------------------------------------------*/

#define NSTAT 100

#define MAXPATHLEN 512

struct CDSpellDEnt;

int   numwords                          = 0;
char *dictionary_file                   = nullptr;
char *hash_file                         = nullptr;
char  dictionary_count_file[MAXPATHLEN] = "";
char  dictionary_stat_file [MAXPATHLEN] = "";

/*----------------------------------------------------------------*/

int          main
              (int, char **);
static void  CSpellCreateNewCountFile
              ();
static void  CSpellReadDictionaryFile
              ();
static int   CSpellMakeDictionaryEntry
              (char *, CDSpellDEnt *);
static void  CSpellFillHashTable
              ();
static void  CSpellWriteHashFile
              ();

#endif
