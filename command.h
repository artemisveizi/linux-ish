/*--------------------------------------------------------------------*/
/* command.h                                                          */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#ifndef COMMAND_INCLUDED
#define COMMAND_INCLUDED

#include"token.h"
#include"dynarray.h"

/* A command is either a built-in or an external command. */
enum CommandType {BUILTIN, EXTERNAL};
/* A COMMAND IS...... */

typedef struct Command *Command_W;


/*--------------------------------------------------------------------*/

/* Return the type of oCommand. */

enum CommandType Command_getType(Command_W oCommand);

/*--------------------------------------------------------------------*/

/* Return the name of oCommand. */

Token_W Command_getName(Command_W oCommand);
/*--------------------------------------------------------------------*/

/* Return the DynArray containing the arguments (tokens) of oCommand. */

DynArray_T Command_getArgs(Command_W oCommand);
/*--------------------------------------------------------------------*/

/* Return the stdin redirect of oCommand. */

Token_W Command_getStdIn(Command_W oCommand);
/*--------------------------------------------------------------------*/

/* Return the stdout redirect of oCommand. */

Token_W Command_getStdOut(Command_W oCommand);

/*--------------------------------------------------------------------*/

/* Return the number of arguments oCommand has (including name). */

size_t Command_argLength(Command_W oCommand);

/*--------------------------------------------------------------------*/

/* Print oCommand to stdout with specified format. Use pcPgmName to
   write descriptive error message to stderr. Formatted according to 
   requirements for ishsyn.c, but can provide useful information for any
   client. */

void Command_writeCommand(Command_W oCommand, const char *pcPgmName);

/*--------------------------------------------------------------------*/

/* Return a command with name oName, argument DynArray oArgs, and stdin/
   stdout redirects stdInToken and stdOutToken. Return NULL if not
   enough memory for command. */

Command_W Command_new(Token_W oName, DynArray_T oArgs,
                      Token_W stdInToken, Token_W stdOutToken);

/*--------------------------------------------------------------------*/

/* Free all memory allocated for oCommand. */

void Command_free(Command_W oCommand);

#endif
