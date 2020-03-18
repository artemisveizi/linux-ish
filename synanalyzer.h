/*--------------------------------------------------------------------*/
/* synanalyzer.h                                                      */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#ifndef SYN_INCLUDED
#define SYN_INCLUDED

#include"dynarray.h"
#include"command.h"

/* Syntactically analyzes DynArray oTokens generated from user input
   by creating a Command corresponding to the given Tokens. Returns the
   generated Command. Writes descriptive error message with pcPgmName if
   unable to generate command. */
Command_W SA_analyzeCommands(DynArray_T oTokens, const char *pcPgmName);

#endif
