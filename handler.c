/*--------------------------------------------------------------------*/
/* handler.c                                                          */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/
#define _GNU_SOURCE

#include"command.h"
#include"dynarray.h"
#include"token.h"
#include<string.h>
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<signal.h>

/* To set file permissions appropriately in stdin/stdout redirects. */
enum{PERMISSIONS = 0600};

void Handler_external(char *oName, char **apcArgv, Command_W oCommand,
                      const char *pcPgmName){
   int iRet;
   pid_t iPid;
   
   assert(oName != NULL && apcArgv != NULL && oCommand != NULL &&
          pcPgmName != NULL);

   /* flush stdin and stdout buffers */
   iRet = fflush(stdin);
   if (iRet == EOF) {perror(pcPgmName); exit(EXIT_FAILURE); }
   iRet = fflush(stdout);
   if (iRet == EOF) {perror(pcPgmName); exit(EXIT_FAILURE); }

   /* fork parent, wait for child process to execute */
   iPid = fork();
   if (iPid == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }

   /* child process identifies own pid 0, executes external command */
   if(iPid == 0){
      Token_W oIn, oOut;
      int inFile, outFile;
      void (*pfRet)(int);
      
      /* restore default signal handler (child process should exit) */
      pfRet = signal(SIGINT, SIG_DFL);
      if(pfRet == SIG_ERR){perror(pcPgmName); exit(EXIT_FAILURE);}
      
      /* handle stdio redirection */
      oIn = Command_getStdIn(oCommand);
      oOut = Command_getStdOut(oCommand);

      /* set up stdin for child */
      if(oIn != NULL){
         inFile = open(Token_getWord(oIn), O_RDONLY);
         if(inFile == -1){perror(pcPgmName); exit(EXIT_FAILURE);}

         iRet = close(0);
         if(iRet == -1){perror(pcPgmName); exit(EXIT_FAILURE);}

         iRet = dup(inFile);
         if(iRet == -1){perror(pcPgmName); exit(EXIT_FAILURE);}

         iRet = close(inFile);
         if(iRet == -1){perror(pcPgmName); exit(EXIT_FAILURE);}
      }
      /* set up stdout for child*/
      if(oOut != NULL){
         outFile = creat(Token_getWord(oOut), PERMISSIONS);
         if(outFile == -1){perror(pcPgmName); exit(EXIT_FAILURE);}

         iRet = close(1);
         if(iRet == -1){perror(pcPgmName); exit(EXIT_FAILURE);}

         iRet = dup(outFile);
         if(iRet == -1){perror(pcPgmName); exit(EXIT_FAILURE);}

         iRet = close(outFile);
         if(iRet == -1){perror(pcPgmName); exit(EXIT_FAILURE);}
      }

      /* execute command in child process */
      if(execvp(oName, apcArgv) < 0){
         perror(pcPgmName); exit(EXIT_FAILURE);
      }

      perror(pcPgmName);
      exit(EXIT_FAILURE);
   }
   /* parent should wait for child process to execute */
   iPid = wait(NULL);
   if(iPid == -1){perror(pcPgmName); exit(EXIT_FAILURE);}  
}

int Handler_builtin(char *oName, char **apcArgv, Command_W oCommand,
                      const char *pcPgmName){
   size_t args = Command_argLength(oCommand);
   
   assert(oName != NULL && apcArgv != NULL && oCommand != NULL &&
          pcPgmName != NULL);

   /* setenv built-in */
   if(strcmp(oName, "setenv") == 0){
      /* must have at least one argument, no more than 2 */
      if(apcArgv[1] == NULL){
         fprintf(stderr, "%s: missing variable\n", pcPgmName);
         return 1;
      }
      if(args > 3){
         fprintf(stderr, "%s: too many arguments\n", pcPgmName);
         return 1;
      }

      /* if no var is given to set env, use "" */
      if(apcArgv[2] == NULL){
         if(setenv(apcArgv[1], "", 1) < 0)
         {perror(pcPgmName); exit(EXIT_FAILURE);}
      }
      /* call setenv with user-inputted var */
      else{
         if(setenv(apcArgv[1], apcArgv[2], 1) < 0)
         {perror(pcPgmName); exit(EXIT_FAILURE);}
      }
      
      return 1;
   }

   /* unsetenv built-in */
   else if(strcmp(oName, "unsetenv") == 0){
      /* must have one argument only */
      if(apcArgv[1] == NULL){
         fprintf(stderr, "%s: missing variable\n", pcPgmName);
         return 1;
      }
      if(args > 2){
         fprintf(stderr, "%s: too many arguments\n", pcPgmName);
         return 1;
      }
      /* call unsetenv with user-inputted env parameter */
      if(unsetenv(apcArgv[1]) < 0)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

      return 1;
   }

   /* cd built-in */
   else if(strcmp(oName, "cd") == 0){
      if(apcArgv[1] == NULL){
         /* check if home environment is set. if not, too few args */
         if(getenv("HOME") != NULL){
            if(chdir(getenv("HOME")) < 0)
            {perror(pcPgmName); exit(EXIT_FAILURE); }
         }
         else{
            fprintf(stderr, "%s: no HOME environment set\n", pcPgmName); 
            return 1;
         }
      }
      /* at most one argument should be given */
      else if(apcArgv[2] != NULL){
         fprintf(stderr, "%s: too many arguments\n", pcPgmName);
         return 1;
      }
      /* call chdir with user-inputted directory parameter */
      else if(chdir(apcArgv[1]) < 0) {
         perror(pcPgmName);
         return 1;
      }
      return 1;
   }

   /* exit built-in */
   else if(strcmp(oName, "exit") == 0){
      return 0;
   }

   return 1;
}
