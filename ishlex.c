/*--------------------------------------------------------------------*/
/* ishlex.c                                                          */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#include"lexanalyzer.h"
#include"token.h"
#include"dynarray.h"
#include<stdio.h>
#include<assert.h>

/* Writes a lexical analysis of user input to stdout. argc and argv
   from stdin (user input). Returns 0 on exit. */
int main(int argc, char *argv[]){
   char *pcLine;
   DynArray_T oTokens;
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
         /* Write lexical analysis to stdout */
         LA_writeTokens(oTokens);
         /* Free dynamically allocated memory */
         LA_freeTokens(oTokens);
         DynArray_free(oTokens);
      }
      LA_freeLine(pcLine);
      printf("%% ");
   }
   printf("\n");
   
   return 0;
}
