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
* struct COMMAND
* To hold details for individual command or argument(s)
*/
typedef struct command {

  char name[MAX_FILE_LENGTH];
  // char args[MAX_ARGS][MAX_FILE_LENGTH];
  char * args[MAX_ARGS];
  int numargs;

  char finname[MAX_FILE_LENGTH];
  char finpath[MAX_LINE_LENGTH];
  char foutname[MAX_FILE_LENGTH];
  char foutpath[MAX_LINE_LENGTH];
  FILE * fsin;
  FILE * fsout;

  bool isBg;                      // Background Process??
  bool isOp;                      // Operator such as '<', '>' or '&'
  char op;                        // Actual char for operator

  void (*free) (struct command *);

} Command;


/*
* struct COMMANDLINE
* To hold all commands for a given line
*/
typedef struct commandline {

  char input[MAX_LINE_LENGTH];
  struct command * commands[MAX_ARGS];

} CommandLine;


void freeCommand(Command * c);
char * substring(char * str, int pos, int len);
void expandVariable(char * input);




/*
* Initializes Command struct
*/
void initCommand(Command * c)
{
  memset(c->name, '\0', MAX_FILE_LENGTH * sizeof(char));

  for (int i = 0; i < MAX_ARGS; i++)
  {
    c->args[i] = NULL;
  }
  c->numargs = 0;
  
  memset(c->finname, '\0', MAX_FILE_LENGTH * sizeof(char));
  memset(c->finpath, '\0', MAX_LINE_LENGTH * sizeof(char));
  memset(c->foutname, '\0', MAX_FILE_LENGTH * sizeof(char));
  memset(c->foutpath, '\0', MAX_LINE_LENGTH * sizeof(char));
  c->fsin = NULL;
  c->fsout = NULL;

  c->isBg = false;
  c->isOp = false;
  c->op = '\0';

  c->free = &freeCommand;
}



/*
* Creates command struct and populates fields based on tokens
*/
Command * createCommand(char * input)
{
  Command * c = malloc(sizeof(Command));
  initCommand(c);

  char * saveptr;

  // Get first toke = name
  char * token = strtok_r(input, " ", &saveptr);
  // strcpy(c->name, expandVariable(token));
  strcpy(c->name, token);

  int count = 0;
  token = strtok_r(NULL, " ", &saveptr);
  while(token != NULL) {
    c->args[count] = malloc(strlen(token) + 1 * sizeof(char));
    strcpy(c->args[count], token);
    token = strtok_r(NULL, " ", &saveptr);
    count++;
  }
  c->numargs = count;

  return c;
}



/*
* Frees allocated memory for a Command struct
*/
void freeCommand(Command * c)
{
  for (int i = 0; i < c->numargs; i++)
  {
    free(c->args[i]);
  }
  free(c);
  c = NULL;
}






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


/*
* Returns true if only characters in string are spaces.
*/
bool hasSpacesOnly(const char * input) {
  int i = 0;
  bool spacesOnly = true;
  while (input[i] != '\0')
  {
    if(input[i] != ' ') {
      spacesOnly = false;
    }
    i++;
  }
  return spacesOnly;
  
}


//-------------------------------------------------
    // Variable Exapansion of '$$'

void expandVariable(char * in)
{
  char * input = malloc(sizeof(char) * strlen(in));
  strcpy(input, in);

  char* replacement = getenv("PID");

  int sizeInput = strlen(input);
  int sizeReplacement = strlen(replacement);

  bool flag = false;


  char * beg = NULL;
  char * end = NULL;
  char * output = NULL;

  for (int i = 0; i < sizeInput; i++)
  {
    if(input[i] == '$' && input[i + 1] == '$') {

      flag = true;

      beg = substring(input, 0, i);
      end = substring(input, i+2, sizeInput - i);
      printf("BEG: %s, PID: %s, END: %s\n", beg, replacement, end);

      output = malloc(sizeof(char) * (sizeInput + 1 + sizeReplacement));
      memset(output, '\0', (sizeof(char) * (sizeInput + 1 + sizeReplacement)));

      strcat(output, beg);
      strcat(output, replacement);
      strcat(output, end);

      free(beg);
      free(end);

      printf("EXAPNDED INPUT: %s\n", output);
      free(input);
      input = output;

      output = NULL;
      sizeInput = strlen(input);
    }
  }

  free(input);
  // return input;
  // 1. Allocate memory for new string
  // 2. strcat last half of input string to new string
  // 3. convert PID to string
  // 4. Add PID as string to input at i with strcat
  // 5. Add remainder of string with strcat
  // 6. Free memory for temporary string
  //-------------------------------------------------

}






/*
* Creates a new substring from a given string starting at position pos of len chars
* SOURCE: https://bit.ly/3IAdLPF, Date: 1/25/22, Adopted
*/
char * substring(char * str, int pos, int len)
{
  char * substring;
  int i;

  substring = malloc(sizeof(char) * (len + 1));
  memset(substring, '\0', (len + 1) * sizeof(char));

  if(substring == NULL) {
    exit(EXIT_FAILURE);
  }

  for ( i = 0; i < len; i++)
  {
    *(substring + i) = *((str + pos) + i);
  }

  return substring;
}


// void expandVariable$$(char * input, char * replacement)
// {
//   int size = strlen(input);
//   char * temp = malloc(sizeof(char) * (size + 1));
//   int sizeReplacement = strlen(replacement);

//   for (int i = 0; i < size; i++)
//   {
//     if(input[i] == '$' && input[i + 1] == '$') {
//       // printf("PID: %d\n", stoi(replacement));
//       printf("PID STR: %s\n", replacement);

//       // strcpy()


//     }
//   }

//   free(temp);

//   // 1. Allocate memory for new string
//   // 2. strcat last half of input string to new string
//   // 3. convert PID to string
//   // 4. Add PID as string to input at i with strcat
//   // 5. Add remainder of string with strcat
//   // 6. Free memory for temporary string
//   //-------------------------------------------------

// }



#endif