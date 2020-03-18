/*--------------------------------------------------------------------*/
/* token.h                                                            */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#ifndef TOKEN_INCLUDED
#define TOKEN_INCLUDED

#include<stdlib.h>
/* A token is either special or unspecial */
enum TokenStatus {ORDINARY, SPECIAL};

/* A token is a word and its associated status. The word is either a
   continuous sequence of characters with no space, or is any sequence
   of characters wrapped with two double quotes (a string). All tokens 
   are unspecial, except for the stdin-redirect and stdout-redirect
   tokens, which are defined by words '<' and '>', respectively. */

typedef struct Token *Token_W;

/*--------------------------------------------------------------------*/

/* Return the status of oToken. */

enum TokenStatus Token_getStatus(Token_W oToken);

/*--------------------------------------------------------------------*/

/* Return the word associated with oToken. */

char *Token_getWord(Token_W oToken);

/*--------------------------------------------------------------------*/

/* Create and return a token whose status is eTokenStatus and whose
   value consists of string pcValue. */

Token_W Token_new(enum TokenStatus eTokenStatus, char *pcValue);

/*--------------------------------------------------------------------*/

/* Free memory associated with token oToken */

void Token_free(Token_W oToken);


#endif
