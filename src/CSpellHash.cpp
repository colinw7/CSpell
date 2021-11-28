#include <CSpellHash.h>

/*------------------------------------------------------------------*
 *
 * #NAME CSpellHash
 *
 * #FUNCTION
 *       Calculate Hash Value for Supplied String.
 *
 * #CALL_DETAILS
 *       hash = CSpellHash(str, length, hashsize);
 *
 * #INPUT_ARGS
 *       str           : The String for which the Hash Value
 *        char *       : is to be calculated.
 *
 *       length        : The length of the String.
 *        int          :
 *
 *       hashsize      : The size of the Hash Table being
 *        int          : used.
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       hash          : The Calculated Hash Value.
 *        unsigned int :
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

extern unsigned int
CSpellHash(char *str, int length, int hash_size)
{
  short hash = 0;

  while (length--) {
    hash ^= *str++;

    if (hash < 0) {
      hash <<= 1;

      hash++;
    }
    else
      hash <<= 1;
  }

  hash &= 077777;

  if (hash_size == 0)
    return 0;

  return ((unsigned int) hash % hash_size);
}
