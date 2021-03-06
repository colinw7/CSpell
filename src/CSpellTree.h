#ifndef CSPELL_TREE_I_H
#define CSPELL_TREE_I_H

#include <CSpellI.h>

/*****************************************************************************/

/* Private Routines */

extern void         CSpellInitTree    (char *);
extern void         CSpellTermTree    ();
extern CDSpellDEnt *CSpellInsertInTree(const char *, int);
extern CDSpellDEnt *CSpellTreeLookup  (char *);
#ifdef NEVER
extern void         CScriptTreeOutput ();
#endif

/*****************************************************************************/

#endif
