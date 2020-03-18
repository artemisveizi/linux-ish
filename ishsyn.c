/*--------------------------------------------------------------------*/
/* ishsyn.c                                                           */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#include"lexanalyzer.h"
#include"synanalyzer.h"
#include"token.h"
#include"command.h"
#include"dynarray.h"
#include<stdio.h>
#include<assert.h>

/* Writes a syntactic analysis of user input to stdout. argc and argv
   from stdin (user input). Returns 0 on exit. */
int main(int argc, char *argv[]){
   char *pcLine;
   DynArray_T oTokens;
   Command_W oCommand;
   int iRet;
   const char *pcPgmName = argv[0];

   printf("%% ");

   /* Read user input line in */
   while((pcLine = LA_readLine(stdin, pcPgmName)) != NULL)
   {
      /* Print it to stdout and flush the stdout buffer */
      printf("%s\n", pcLine);
      iRet = fflush(stdout);

      if(iRet == EOF)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Lexically analyze user input */
      oTokens = LA_lexLine(pcLine, pcPgmName);

      if(oTokens != NULL)
      {
         /* Syntactically analyze user input */
         oCommand = SA_analyzeCommands(oTokens, pcPgmName);
         if(oCommand != NULL){
            /* Write syntactic analysis to stdout */
            Command_writeCommand(oCommand, pcPgmName);
            Command_free(oCommand);
         }
         /* Free dynamically allocated memory */
         LA_freeTokens(oTokens);
         DynArray_free(oTokens);
      }
      LA_freeLine(pcLine);

      /* Continue, waiting for new user input after previous executed */
      printf("%% ");
      iRet = fflush(stdout);

      if(iRet == EOF)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   }
   printf("\n");
   iRet = fflush(stdout);

   if(iRet == EOF)
   {perror(pcPgmName); exit(EXIT_FAILURE);}
   return 0;
}
