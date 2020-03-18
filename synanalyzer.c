/*--------------------------------------------------------------------*/
/* synanalyzer.c                                                      */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#include"dynarray.h"
#include"token.h"
#include"command.h"
#include<stdio.h>
#include<assert.h>
#include<string.h>

Command_W SA_analyzeCommands(DynArray_T oTokens, const char *pcPgmName){
   Command_W oCommand;
   
   size_t u, uNext;
   size_t uLength;
   Token_W psToken, psTokenNext;
   enum TokenStatus psStatus;

   Token_W oName;
   DynArray_T oArgs;
   
   int dirIn = 0;
   int dirOut = 0;
   Token_W stdInToken = NULL;
   Token_W stdOutToken = NULL;

   int iSuccessful;
   
   assert(oTokens != NULL && pcPgmName != NULL);
   
   uLength = DynArray_getLength(oTokens);
   /* Create an empty token DynArray object. */
   oArgs = DynArray_new(0);
   if(oArgs == NULL)
   {perror(pcPgmName); exit(EXIT_FAILURE);}
   
   /* Get the first token, ensure ordinary, print */
   if(uLength > 0){
      oName = DynArray_get(oTokens, 0);
      if(Token_getStatus(oName) != ORDINARY){
         fprintf(stderr, "%s: missing command name\n", pcPgmName);
         DynArray_free(oArgs);
         return NULL;
      }
      
      /* Put the remaining tokens either into the args array for the
         command... */
      for(u = 1; u < uLength; u++)
      {
         psToken = DynArray_get(oTokens, u);
         psStatus = Token_getStatus(psToken);
         if(psStatus == ORDINARY){
            iSuccessful = DynArray_add(oArgs, psToken);
            if(!iSuccessful){perror(pcPgmName); exit(EXIT_FAILURE);}
         }
         /* Or into the stdin/stdout tokens for the command. */
         else{
            /* stdout */
            if(strcmp(Token_getWord(psToken), ">") == 0){
               if(dirOut != 0){
                  fprintf(stderr,
                          "%s: multiple redirection of standard output\n",
                          pcPgmName);
                  DynArray_free(oArgs);
                  return NULL;
               }

               /* Ensure an argument follows the stdout redirect. */
               uNext = u + 1;
               if(uNext == uLength){
                  fprintf(stderr,
                          "%s: standard output redirection without file name\n",
                          pcPgmName);
                  DynArray_free(oArgs);
                  return NULL;
               }
               psTokenNext = DynArray_get(oTokens, uNext);
               stdOutToken = psTokenNext;
               dirOut++;
               u = uNext;
            }
            /* stdin */
            else if(strcmp(Token_getWord(psToken), "<") == 0){
               if(dirIn != 0){
                  fprintf(stderr,
                          "%s: multiple redirection of standard input\n",
                          pcPgmName);
                  DynArray_free(oArgs);
                  return NULL;
               }

               /* Ensure an argument follows the stdin reirect. */
               uNext = u + 1;
               if(uNext == uLength){
                  fprintf(stderr,
                          "%s: standard input redirection without file name\n",
                          pcPgmName);
                  DynArray_free(oArgs);
                  return NULL;
               }
               psTokenNext = DynArray_get(oTokens, uNext);
               stdInToken = psTokenNext;
               dirIn++;
               u = uNext;
            }
            else{
               fprintf(stderr, "Invalid token\n");
               DynArray_free(oArgs);
               return NULL;
            }
         
         }
      }
      /* Create command with newly analyzed parameters */
      oCommand = Command_new(oName, oArgs, stdInToken, stdOutToken);

      if(oCommand == NULL)
      {
         DynArray_free(oArgs);
         perror(pcPgmName);
         exit(EXIT_FAILURE);
      }

      return oCommand;
   }
   DynArray_free(oArgs);
   
   return NULL;
}
