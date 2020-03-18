/*--------------------------------------------------------------------*/
/* token.c                                                            */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#include "token.h"
#include <stddef.h>
#include <assert.h>
#include <string.h>

/*--------------------------------------------------------------------*/
/* A token is a structure consisting of a "word" and a status (special 
   or not special). */
struct Token
{
   /* The status of the Token. */
   enum TokenStatus tStatus;
   
   /* A char array consisting of the word */
   char *tWord;
};

enum TokenStatus Token_getStatus(Token_W oToken){
   assert(oToken != NULL);
   
   return oToken->tStatus;
}

char *Token_getWord(Token_W oToken){
   assert(oToken != NULL);

   return oToken->tWord;
}

Token_W Token_new(enum TokenStatus eTokenStatus, char *pcValue){
   Token_W psToken;
   char *pcWord;

   assert(pcValue != NULL);

   psToken = (Token_W)malloc(sizeof(struct Token));
   if (psToken == NULL) {
      return NULL;
   }

   /* Set struct fields */
   psToken->tStatus = eTokenStatus;
   pcWord = (char*)malloc(strlen(pcValue) + 1);
   if (pcWord == NULL){
      return NULL;
   }
   psToken->tWord = pcWord;
   strcpy(pcWord, pcValue);

   return psToken;
}

void Token_free(Token_W oToken){
   assert(oToken != NULL);

   free(Token_getWord(oToken));
   free(oToken);
}
