/*--------------------------------------------------------------------*/
/* handler.h                                                          */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/

#ifndef HANDLER_INCLUDED
#define HANDLER_INCLUDED

#include"command.h"

/* Handles external commands from user input by forking from parent 
   process and running appropriate command. oName is the command name, 
   apcArgv is the argument array, oCommand is the command itself, and
   pcPgmName is used to write descriptive error messages when errno is
   set by failed method calls. */
void Handler_external(char *oName, char **apcArgv, Command_W oCommand,
                      const char *pcPgmName);

/* Handles built in commands from user input by forking from parent
   process and running appropriate command. oName is the command name,
   apcArgv is the argument array, oCommand is the command itself, and
   pcPgmName is used to write descriptive error messages when user input
   is erroneous or errno is set by failed method calls.
   Returns 0 on exit command, else 1. */

int Handler_builtin(char *oName, char **apcArgv, Command_W oCommand,
                      const char *pcPgmName);

#endif
