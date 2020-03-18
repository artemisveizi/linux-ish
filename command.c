/*--------------------------------------------------------------------*/
/* command.c                                                          */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#include"command.h"
#include"token.h"
#include"dynarray.h"
#include<assert.h>
#include<stddef.h>
#include<stdio.h>
#include<string.h>

/* A command is a struct possessing a type, a name token, a DynArray of
   arguments, and stdin and stdout redirect tokens.*/
struct Command
{
   /* The type of the command */
   enum CommandType cType;

   /* The name of the command */
   Token_W cName;
   
   /* A DynArray consisting of all arguments of the command */
   DynArray_T cArgs;
   
   /* The stdin redirect of the command */
   Token_W cStdIn;
   
   /* The stdout redirect of the command */
   Token_W cStdOut;
};

enum CommandType Command_getType(Command_W oCommand){
   assert(oCommand != NULL);
   
   return oCommand->cType;
}

Token_W Command_getName(Command_W oCommand){
   assert(oCommand != NULL);
   
   return oCommand->cName;
}
      
DynArray_T Command_getArgs(Command_W oCommand){
   assert(oCommand != NULL);

   return oCommand->cArgs;
}

Token_W Command_getStdIn(Command_W oCommand){
   assert(oCommand != NULL);

   return oCommand->cStdIn;   
}

Token_W Command_getStdOut(Command_W oCommand){
   assert(oCommand != NULL);

   return oCommand->cStdOut; 
}

/* Sets the name of oCommand to oName and determines and sets the 
   associated type. Cannot be used by any clients. */
static void Command_setName(Command_W oCommand, Token_W oName){
   char *oNameWord;
   assert(oCommand != NULL && oName != NULL);

   oCommand->cName = oName;
   oNameWord = Token_getWord(oName);
   if(strcmp(oNameWord, "setenv") == 0 ||
      strcmp(oNameWord, "unsetenv") == 0 ||
      strcmp(oNameWord, "cd") == 0 ||
      strcmp(oNameWord, "exit") == 0){
      oCommand->cType = BUILTIN;
   }
   else
      oCommand->cType = EXTERNAL;
}

size_t Command_argLength(Command_W oCommand){
   size_t length = 1;
   assert(oCommand != NULL);

   length += DynArray_getLength(Command_getArgs(oCommand));

   return length;
}

void Command_writeCommand(Command_W oCommand, const char *pcPgmName){
   size_t u, uLength;
   Token_W psToken;
   Token_W oName, oIn, oOut;
   DynArray_T oArgs;
   int iRet;

   assert(oCommand != NULL && pcPgmName != NULL);

   oName = Command_getName(oCommand);
      
   oArgs = Command_getArgs(oCommand);
   uLength = DynArray_getLength(oArgs);

   oIn = Command_getStdIn(oCommand);
   oOut = Command_getStdOut(oCommand);
   
   /* Write command name first */
   printf("Command name: %s\n", Token_getWord(oName));
   iRet = fflush(stdout);
   if(iRet == EOF)
   {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Write all arguments before stdin and stdout info */
   for(u = 0; u < uLength; u++){
      psToken = DynArray_get(oArgs, u);
      printf("Command arg: %s\n", Token_getWord(psToken));
      iRet = fflush(stdout);
      if(iRet == EOF)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   }

   /* Write command stdin and stdout */
   if(oIn != NULL){
      printf("Command stdin: %s\n", Token_getWord(oIn));
      iRet = fflush(stdout);
      if(iRet == EOF)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   }
   if(oOut != NULL){
      printf("Command stdout: %s\n", Token_getWord(oOut));
      iRet = fflush(stdout);
      if(iRet == EOF)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   }
}

Command_W Command_new(Token_W oName, DynArray_T oArgs,
                      Token_W stdInToken, Token_W stdOutToken){
   Command_W oCommand;

   assert(oName != NULL && oArgs != NULL);

   oCommand = (Command_W)malloc(sizeof(struct Command));
   if(oCommand == NULL)
   {
      return NULL;
   }

   Command_setName(oCommand, oName);
   oCommand->cArgs = oArgs;
   oCommand->cStdIn = stdInToken;
   oCommand->cStdOut = stdOutToken;

   return oCommand;
}

void Command_free(Command_W oCommand){
   assert(oCommand != NULL);
   
   DynArray_free(oCommand->cArgs);
   free(oCommand);
}
