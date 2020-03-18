/*--------------------------------------------------------------------*/
/* lexanalyzer.c                                                      */
/* Author: Artemis Veizi                                              */
/*--------------------------------------------------------------------*/
#include"lexanalyzer.h"
#include"dynarray.h"
#include"token.h"
#include<ctype.h>
#include<stdio.h>
#include<stddef.h>
#include<assert.h>
#include<stdlib.h>
#include<string.h>

char *LA_readLine(FILE *psFile, const char *pcPgmName)
{
   const size_t INITIAL_LINE_LENGTH = 2;
   const size_t GROWTH_FACTOR = 2;

   size_t uLineLength = 0;
   size_t uPhysLineLength = INITIAL_LINE_LENGTH;
   char *pcLine;
   int iChar;

   assert(psFile != NULL && pcPgmName != NULL);

   /* If no lines remain, return NULL. */
   if (feof(psFile))
      return NULL;
   iChar = fgetc(psFile);
   if (iChar == EOF)
      return NULL;

   /* Allocate memory for the string. */
   pcLine = (char*)malloc(uPhysLineLength);
   if (pcLine == NULL)
   {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Read characters into the string. */
   while ((iChar != '\n') && (iChar != EOF))
   {
      if (uLineLength == uPhysLineLength)
      {
         uPhysLineLength *= GROWTH_FACTOR;
         pcLine = (char*)realloc(pcLine, uPhysLineLength);
         if (pcLine == NULL)
         {perror(pcPgmName); exit(EXIT_FAILURE);}
         
      }
      pcLine[uLineLength] = (char)iChar;
      uLineLength++;
      iChar = fgetc(psFile);
   }

   /* Append a null character to the string. */
   if (uLineLength == uPhysLineLength)
   {
      uPhysLineLength++;
      pcLine = (char*)realloc(pcLine, uPhysLineLength);
      if (pcLine == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
      
   }
   pcLine[uLineLength] = '\0';

   return pcLine;
}

void LA_freeLine(char *pcLine){
   assert(pcLine != NULL);
   
   free(pcLine);
}

void LA_freeTokens(DynArray_T oTokens)
{
   size_t u;
   size_t uLength;
   Token_W psToken;

   assert(oTokens != NULL);

   uLength = DynArray_getLength(oTokens);

   for (u = 0; u < uLength; u++)
   {
      psToken = DynArray_get(oTokens, u);
      Token_free(psToken);
   }
}

void LA_writeTokens(DynArray_T oTokens)
{
   size_t u;
   size_t uLength;
   Token_W psToken;

   assert(oTokens != NULL);

   uLength = DynArray_getLength(oTokens);

   for (u = 0; u < uLength; u++)
   {
      printf("Token: ");
      psToken = DynArray_get(oTokens, u);
      printf("%s ", Token_getWord(psToken));
      if (Token_getStatus(psToken) == ORDINARY)
         printf("(ordinary)\n");
      else
         printf("(special)\n");
   }
}

DynArray_T LA_lexLine(const char *pcLine, const char *pcPgmName)
{
   /* lexLine() uses a DFA approach.  It "reads" its characters from
      pcLine. The DFA has these three states: */
   enum LexState {STATE_START, STATE_IN_STRING, STATE_IN_WORD};

   /* The current state of the DFA. */
   enum LexState eState = STATE_START;

   /* An index into pcLine. */
   size_t uLineIndex = 0;

   /* Pointer to a buffer in which the characters comprising each
      token are accumulated. */
   char *pcBuffer;

   /* An index into the buffer. */
   int uBufferIndex = 0;

   char c;
   Token_W psToken;
   DynArray_T oTokens;
   int iSuccessful;
   
   assert(pcLine != NULL && pcPgmName != NULL);

   /* Create an empty token DynArray object. */
   oTokens = DynArray_new(0);
   if (oTokens == NULL)
   {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Allocate memory for a buffer that is large enough to store the
      largest token that might appear within pcLine. */
   pcBuffer = (char*)malloc(strlen(pcLine) + 1);
   if (pcBuffer == NULL)
   {perror(pcPgmName); exit(EXIT_FAILURE);}
   
   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[uLineIndex++];

      switch (eState)
      {
         /* Handle the START state. */
         case STATE_START:
            if (c == '\0')
            {
               free(pcBuffer);
               return oTokens;
            }
            /* If a string quote appears, do not add it to the buffer,
               but change the state to IN STRING. */
            else if (c == '\"')
            {
               eState = STATE_IN_STRING;
            }
            /* If these characters comprise special tokens, ignore any 
               adjoined characters. Create a special token with the 
               character and add it to the tokens dynarray */
            else if (c == '<' || c == '>')
            {
               uBufferIndex = 0;
               pcBuffer[uBufferIndex++] = c;
               pcBuffer[uBufferIndex] = '\0';
               psToken = Token_new(SPECIAL, pcBuffer);
               if(psToken == NULL){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               iSuccessful = DynArray_add(oTokens, psToken);
               if (!iSuccessful){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               uBufferIndex = 0;
               eState = STATE_START;
            }
            else if (isspace(c))
               eState = STATE_START;
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_IN_WORD;
            }
            break;

            /* Handle the IN_STRING state. */
         case STATE_IN_STRING:
            if (c == '\0')
            {
               /* Unterminated string illegal. */
               fprintf(stderr, "%s: unmatched quote\n", pcPgmName);
               free(pcBuffer);
               LA_freeTokens(oTokens);
               DynArray_free(oTokens);
               return NULL;
            }
            else if (c == '\"')
            {
               /* Progress to IN WORD state to ensure full argument is 
                  tokenized. */
               eState = STATE_IN_WORD;
            }
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_IN_STRING;               
            }
            break;

            /* Handle the IN_WORD state. */
         case STATE_IN_WORD:
            if (c == '\0')
            {
               /* Create a WORD token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = Token_new(ORDINARY, pcBuffer);
               if(psToken == NULL){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               iSuccessful = DynArray_add(oTokens, psToken);
               if (!iSuccessful){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               uBufferIndex = 0;
               free(pcBuffer);
               return oTokens;
            }
            /* Progress to IN STRING state to ensure full argument is
               tokenized. */
            else if (c == '\"')
            {
               eState = STATE_IN_STRING;
            }
            /* Same special token handling as in state state. */
            else if (c == '<' || c == '>')
            {
               pcBuffer[uBufferIndex] = '\0';
               psToken = Token_new(ORDINARY, pcBuffer);
               if(psToken == NULL){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               iSuccessful = DynArray_add(oTokens, psToken);
               if (!iSuccessful){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               uBufferIndex = 0;
               pcBuffer[uBufferIndex++] = c;
               pcBuffer[uBufferIndex] = '\0';
               psToken = Token_new(SPECIAL, pcBuffer);
               if(psToken == NULL){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               iSuccessful = DynArray_add(oTokens, psToken);
               if (!iSuccessful){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               uBufferIndex = 0;
               eState = STATE_START;
            }
            else if (isspace(c))
            {
               /* Create a WORD token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = Token_new(ORDINARY, pcBuffer);
               if(psToken == NULL){perror(pcPgmName); exit(EXIT_FAILURE);}
               
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful){perror(pcPgmName); exit(EXIT_FAILURE);}

               uBufferIndex = 0;
               eState = STATE_START;
            }
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_IN_WORD;
            }
            break;

         default:
            assert(0);
      }
   }
}

            
