/*--------------------------------------------------------------------*/
/* ish.c                                                              */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/
#define _GNU_SOURCE
#include"lexanalyzer.h"
#include"synanalyzer.h"
#include"token.h"
#include"dynarray.h"
#include"command.h"
#include"handler.h"
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

/* SIGINT (iSignal = SIGINT) signal handler for the parent process. */
static void sigParentHandler(int iSignal){
   void (*pfRet)(int);
   int WAIT_SECONDS = 3;
   /* If not the child process, set alarm for 3 sec */
   if(getpid() != 0){
      alarm(WAIT_SECONDS);
      /* Within 3 seconds, all SIGINT signals handled by default  */
      pfRet = signal(SIGINT, SIG_DFL);
      if(pfRet == SIG_ERR){exit(EXIT_FAILURE);}
   }
}

/* SIGALRM (iSignal = SIGALRM) signal handler for the parent process */
static void sigAlrmHandler(int iSignal){
   void (*pfRet)(int);
   /* After 3 seconds, parent process SIGINT handler is restored */
   pfRet = signal(SIGINT, sigParentHandler);
   if(pfRet == SIG_ERR){exit(EXIT_FAILURE);}
}

/* Runs a minimal but realistic interactive Linux shell. argc and argv
   from stdin (user input). Returns 0 on shell exit. */
int main(int argc, char *argv[]){
   char *pcLine;
   DynArray_T oTokens;
   Command_W oCommand;
   enum CommandType oCommandType;

   int biRet;
   int iRet;

   char *oName;
   size_t args, i;
   DynArray_T oArgs;
   char **apcArgv;

   void (*pfRet)(int);
   void (*pfAlrm)(int);
   
   const char *pcPgmName = argv[0];

   /* Set up signal handlers for parent process (inherited by child) */
   pfRet = signal(SIGINT, sigParentHandler);
   if(pfRet == SIG_ERR){perror(pcPgmName); exit(EXIT_FAILURE);}
   pfAlrm = signal(SIGALRM, sigAlrmHandler);
   if(pfAlrm == SIG_ERR){perror(pcPgmName); exit(EXIT_FAILURE);}
      
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
            /* Create argv array for handler to execute user input */
            args = Command_argLength(oCommand);
            
            apcArgv = (char **)malloc((args + 1) * sizeof(char *));
            if(apcArgv == NULL) {perror(pcPgmName); exit(EXIT_FAILURE);}

            oName =  Token_getWord(Command_getName(oCommand));
            apcArgv[0] = oName;

            oArgs = Command_getArgs(oCommand);
            for(i = 1; i < args; i++){
               apcArgv[i] = Token_getWord(DynArray_get(oArgs, i - 1));
            }
            apcArgv[args] = NULL;

            /* Handler executes external and builtin commands 
               differently */
            oCommandType = Command_getType(oCommand);
            switch(oCommandType)
            {
               case EXTERNAL:
                  Handler_external(oName, apcArgv, oCommand, pcPgmName);
                  break;
               case BUILTIN:
                  biRet = Handler_builtin(oName, apcArgv, oCommand, pcPgmName);
                  /* If exit signal is returned from handler, exit */
                  if(biRet == 0){
                     free(apcArgv);
                     Command_free(oCommand);
                     LA_freeTokens(oTokens);
                     DynArray_free(oTokens);
                     LA_freeLine(pcLine);
                     return biRet;
                  }
                  break;
               default:
                  return 0;
            }
            /* Free all dynamically allocated memory */
            free(apcArgv);
            Command_free(oCommand);
         }
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

   /* Shell complete */
   printf("\n");
   iRet = fflush(stdout);

   if(iRet == EOF)
   {perror(pcPgmName); exit(EXIT_FAILURE);}
   
   return 0;
}
