/*--------------------------------------------------------------------*/
/* lexanalyzer.h                                                      */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#ifndef LA_INCLUDED
#define LA_INCLUDED

#include"token.h"
#include"dynarray.h"
#include<stdio.h>
/*--------------------------------------------------------------------*/

/* If no lines remain in psFile, then return NULL. Otherwise read a line
   of psFile and return it as a string. The string does not contain a
   terminating newline character. The caller owns the string. If error
   occurs, descriptive message is written to stderr with pcPgmName. */

char *LA_readLine(FILE *psFile, const char *pcPgmName);

/*--------------------------------------------------------------------*/

/* Free pcLine, generated by LA_readLine. */

void LA_freeLine(char *pcLine);

/*--------------------------------------------------------------------*/

/* Free all of the tokens in oTokens. */

void LA_freeTokens(DynArray_T oTokens);


/*--------------------------------------------------------------------*/

/* Write all tokens in oTokens to stdout.  First write the number
   tokens; then write the word tokens. */

void LA_writeTokens(DynArray_T oTokens);

/*--------------------------------------------------------------------*/

/* Lexically analyzes pcLine, a line of user input by creating Tokens 
   for each distinct "argument" in the input, returning a DynArray of 
   all Tokens generated by input. If error occurs, descriptive message 
   is written to stderr with pcPgmName.  */
DynArray_T LA_lexLine(const char *pcLine, const char *pcPgmName);

#endif