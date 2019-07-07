#ifndef CSPELL_LOOKUP_I_H
#define CSPELL_LOOKUP_I_H

#include <CSpellI.h>

/*****************************************************************************/

/* Private Data */

extern CDSpellDEnt  *hashtbl;
extern int           hashsize;

/* Private Routines */

extern int           CSpellInitWords ();
extern void          CSpellTermWords ();
extern CDSpellDEnt  *CSpellLookupWord(char *, int, int);

/*****************************************************************************/

#endif
