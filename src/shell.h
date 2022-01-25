#ifndef SHELL_H
#define SHELL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 2048
#define MAX_FILE_LENGTH 256
#define MAX_ARGS 512


/*
* Struct to hold details of individual command or argument(s)
*/
typedef struct command {

  char name[MAX_FILE_LENGTH];
  char args[MAX_ARGS][MAX_FILE_LENGTH];

  char finname[MAX_FILE_LENGTH];
  char finpath[MAX_LINE_LENGTH];
  char foutname[MAX_FILE_LENGTH];
  char foutpath[MAX_LINE_LENGTH];
  FILE * fsin;
  FILE * fsout;

  bool isBg;                      // Background Process??
  bool isOp;                      // Operator such as '<', '>' or '&'
  char op;                        // Actual char for operator

} Command;


/*
* Initializes Command struct and clears memory
*/
void initCommand(Command * c)
{
  memset(c->name, '\0', MAX_FILE_LENGTH * sizeof(char));

  for (int i = 0; i < MAX_ARGS; i++)
  {
    memset(c->args[i], '\0', MAX_FILE_LENGTH * sizeof(char));
  }
  
  memset(c->finname, '\0', MAX_FILE_LENGTH * sizeof(char));
  memset(c->finpath, '\0', MAX_LINE_LENGTH * sizeof(char));
  memset(c->foutname, '\0', MAX_FILE_LENGTH * sizeof(char));
  memset(c->foutpath, '\0', MAX_LINE_LENGTH * sizeof(char));
  c->fsin = NULL;
  c->fsout = NULL;

  c->isBg = false;
  c->isOp = false;
  c->op = '\0';
}



/*
* Struct to hold all commands for a given line
*/
typedef struct commandline {

  char input[MAX_LINE_LENGTH];
  struct command * commands[MAX_ARGS];

} CommandLine;


/*
* Initializes CommandLine struct and clears memory
*/
void initCommandLine(CommandLine * cl)
{
  memset(cl->input, '\0', MAX_LINE_LENGTH * sizeof(char));
  // memset(cl->commands, '\0', MAX_ARGS * sizeof(Command));
}


/*
* Displays a colon prompt and flushes the output stream.
*/
void shellPrompt() {
  printf(": ");
  fflush(stdout);
}


/*
* Takes a char array and size as input, Sets content to all '\0',
* gets user input using fgets, and strips the newline character from the array.
*/
void getInput(char * input, int size)
{
  memset(input, '\0', size * sizeof(char));
  fgets(input, size, stdin);
  input[strcspn(input, "\n")] = '\0';  // Remove newline SOURCE: https://bit.ly/3fyCbwz, Date 1/18/22, Adopted
}




#endif