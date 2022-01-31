#ifndef SHELL_H
#define SHELL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>    // for waitpid

#define MAX_LINE_LENGTH 2048
#define MAX_FILE_LENGTH 256
#define MAX_ARGS 512
#define MAX_PIDS 512



/*******************************************************
*
* STRUCTS
*
*******************************************************/

/*
* struct COMMAND
* To hold details for individual command or argument(s)
*/
typedef struct command {

  char name[MAX_FILE_LENGTH];
  char * args[MAX_ARGS];
  int numargs;

  char finname[MAX_FILE_LENGTH];
  char finpath[MAX_LINE_LENGTH];
  char foutname[MAX_FILE_LENGTH];
  char foutpath[MAX_LINE_LENGTH];
  int fdin;
  int fdout;

  bool hasInput;
  bool hasOutput;
  bool isBg;                      // Background Process??

  void (*parseInput) (char *, struct command *);
  void (*parseStreams) (struct command *);
  void (*trimArgs) (struct command *);
  void (*free) (struct command *);
  void (*print) (struct command *);

} Command;


/*
* struct COMMANDLINE
* To hold all commands for a given line
*/
typedef struct commandline {

  char input[MAX_LINE_LENGTH];
  struct command * commands[MAX_ARGS];

} CommandLine;


typedef struct pids {
  int numpids;
  int pids[MAX_PIDS];

  void (*add) (struct pids *, int);
  void (*remove) (struct pids *, int);
  void (*check) (struct pids *);
  void (*kill) (struct pids *);
  void (*print) (struct pids *);
  void (*free) (struct pids *);
} Pids;



/*******************************************************
*
* Function PROTOTYPES
*
*******************************************************/

void shellPrompt();
void getInput(char * input, int size);
bool hasSpacesOnly(const char * input);
Command * createCommand();
void initCommand(Command * c);
void parseCommandInput(char * input, Command * c);
void parseCommandStreams(Command * c);
void trimCommandArgs(Command * c);
void freeCommand(Command * c);
void printCommand(Command * c);
Pids * createPids();
void initPids(Pids * p);
void addPid(Pids * p, int value);
void removePid(Pids * p, int value);
void checkPids(Pids * p);
void killPids(Pids * p);
void printPids(Pids * p);
void freePids(Pids * p);
char * expandVariable(char * input);
char * substring(char * str, int pos, int len);
void trimLeadingWhitespace(char * input);





/*******************************************************
*
* Function DEFINITIONS
*
*******************************************************/
/*
* Creates command struct and initializes it.
* Returns the new struct.
*/
Command * createCommand()
{
  Command * c = malloc(sizeof(Command));
  initCommand(c);

  return c;
}


/*
* Creates pids struct and initializes it.
* Returns the new struct.
*/
Pids * createPids()
{
  Pids * p = malloc(sizeof(Pids));
  initPids(p);

  return p;
}


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
  
  // Default Linux directory to channel I/O streams when not specified
  strcpy(c->foutpath, "/dev/null");
  strcpy(c->finpath, "/dev/null");

  c->fdin = -1;
  c->fdout = -1;

  c->hasInput = false;
  c->hasOutput = false;
  c->isBg = false;

  c->parseInput = &parseCommandInput;
  c->parseStreams = &parseCommandStreams;
  c->trimArgs = &trimCommandArgs;
  c->print = &printCommand;
  c->free = &freeCommand;
}



/*
* Parses an input string into command struct and populates fields based on tokens
*/
void parseCommandInput(char * input, Command * c)
{
  char * saveptr;

  //---------------------------------
  //   NAME
  //---------------------------------
  char * token = strtok_r(input, " ", &saveptr);
  if(token != NULL) {
    token = expandVariable(token);
    strcpy(c->name, token);

    // Copy name of command to first in list of args
    c->args[0] = malloc(strlen(token) + 1 * sizeof(char));
    strcpy(c->args[0], token);


    // Get Arguments
    int argsCount = 1;
    free(token);
    token = strtok_r(NULL, " ", &saveptr);

    while(token != NULL) {
      token = expandVariable(token);   // free(token) if mutated by expandVariable
      
      //---------------------------------
      //   "<" - INPUT
      //---------------------------------
      if(strcmp(token, "<") == 0)
      {
        c->hasInput = true;
        free(token);

        // Get next token
        token = strtok_r(NULL, " ", &saveptr);
        if(token != NULL) {
          token = expandVariable(token);
          strcpy(c->finpath, token);
          strcpy(c->finname, token);
        }
      }

      //---------------------------------
      //   ">" - OUTPUT
      //---------------------------------
      else if (strcmp(token, ">") == 0)
      {
        c->hasOutput = true;
        free(token);

        // Get next token
        token = strtok_r(NULL, " ", &saveptr);
        if(token != NULL) {
          token = expandVariable(token);
          strcpy(c->foutpath, token);
          strcpy(c->foutname, token);
        }
      }

      //---------------------------------
      //   ARGS
      //---------------------------------
      else
      {
        c->args[argsCount] = malloc(strlen(token) + 1 * sizeof(char));
        strcpy(c->args[argsCount], token);
        argsCount++;
      }

      free(token);

      // Get next token
      if(token != NULL) {
        token = strtok_r(NULL, " ", &saveptr);
      }
    }

    c->numargs = argsCount;
  }
}



/*
* Identifies file streams and if process will be in the background.
* Special operators are: <, >, &
*/
void parseCommandStreams(Command * c)
{
  for (int i = 0; i < c->numargs; i++)
  {
    // Input Stream
    if(strcmp(c->args[i], "<") == 0 && (i + 1) < c->numargs)
    {
      strcpy(c->finpath, c->args[i + 1]);
    }

    // Output Stream
    if(strcmp(c->args[i], ">") == 0 && (i + 1) < c->numargs)
    {
      strcpy(c->foutpath, c->args[i + 1]);
    }

    // Background Process
    if(c->numargs > 0 && strcmp(c->args[c->numargs - 1], "&") == 0) {
      c->isBg = true;
    }
  }
}



/*
* Removes args from args list after first special operator encountered.
* Special operators are: <, >, &
*/
void trimCommandArgs(Command * c)
{
  bool flag = false;
  int newnumargs = c->numargs;    // Track new number of args

  for (int i = 0; i < c->numargs; i++)
  {

    // Set for first special operator found
    if( strcmp(c->args[i], "<") == 0 || 
        strcmp(c->args[i], ">") == 0 ||
        strcmp(c->args[i], "&") == 0 )
    {
      flag = true;
    }

    // Remove all args after flag is set
    if(flag) {
      // printf("removing: %s\n", c->args[i]);
      free(c->args[i]);
      c->args[i] = NULL;
      newnumargs--;
    }
  }
  c->numargs = newnumargs;
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
* Prints values of struct command variables
*/
void printCommand(Command * c)
{
  printf("  CMD: %s\n", c->name);
  printf("  ARGS: ");
  fflush(stdout);
  for (int i = 0; i < c->numargs; i++)
  {
    printf("%s ", c->args[i]);
  }
  printf("\n");

  printf("  finname: %s\n", c->finname);
  printf("  finpath: %s\n", c->finpath);
  printf("  foutname: %s\n", c->foutname);
  printf("  foutpath: %s\n", c->foutpath);
  printf("  fsin: %d\n", c->fdin);
  printf("  fsout: %d\n", c->fdin);

  printf("  hasInput: %d\n", c->hasInput);
  printf("  hasOutput: %d\n", c->hasOutput);
  printf("  isBG: %d\n", c->isBg);


  fflush(stdout);
}




/*
* Initializes Pids struct
*/
void initPids(Pids * p)
{
  p->numpids = 0;
  memset(p->pids, 0, MAX_PIDS);

  p->add = &addPid;
  p->remove = &removePid;
  p->check = &checkPids;
  p->kill = &killPids;
  p->print = &printPids;
  p->free = &freePids;
}



/*
* Adds pid value to Pids struct
*/
void addPid(Pids * p, int value)
{
  p->pids[p->numpids] = value;
  p->numpids++;
}




/*
* Removes pid value from Pids struct
*/
void removePid(Pids * p, int value)
{
  bool found = false;
  for (int i=0; i < p->numpids; i++)
  {
    int j = i + 1;

    if(p->pids[i] == value){
      found = true;
      if(j < p->numpids) {
        p->pids[i] = p->pids[j];      
      } else {
        p->pids[i] = 0;
      }
    }
  }

  if(found) {
    p->numpids--;
  }
}



/*
* Prints values of struct Pids
*/
void printPids(Pids * p)
{
  printf("PIDS: \n");
  for (int i = 0; i < p->numpids; i++)
  {
    printf("%d\n", p->pids[i]);
  }
  printf("\n");
}



/*
* Checks each pid in Pids array for current status without waiting (WNOHANG)
* Prints status of pids that finished
*/
void checkPids(Pids * p)
{
  int status = -1;
  int childStatus;
  for (int i = 0; i < p->numpids; i++)
  {
    status = waitpid(p->pids[i], &childStatus, WNOHANG);  // Immediately checks on child status
    if(status != 0) {
      printf("background pid %d is done: ", p->pids[i]);
      if(WIFEXITED(childStatus)){
          printf("exit status %d\n", WEXITSTATUS(childStatus));
          fflush(stdout);
        } else{
          printf("terminated by signal %d\n", WTERMSIG(childStatus));
          fflush(stdout);
        }
      p->remove(p, p->pids[i]);
      p->print(p);
    }
  }
}



/*
* Kills each pid in Pids array
*/
void killPids(Pids * p)
{
  for (int i = p->numpids - 1; i >= 0; i--)
  {
    // SOURCE: https://bit.ly/3gcmbQY, Date: 1/30/22, Adopted
    int ret = kill(p->pids[i], SIGKILL);
    if (ret == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
    printf("Killed child process: %d\n", p->pids[i]);
    removePid(p, p->pids[i]);
    p->print(p);
  }
}




/*
* Frees allocated memory for a Pids struct
*/
void freePids(Pids * p)
{
  // memset(p->pids, 0, MAX_PIDS);

  free(p);
  p = NULL;
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
void shellPrompt() 
{
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
bool hasSpacesOnly(const char * input) 
{
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



/*
* Expands any instance of '$$' to the shell process id.
*/
char * expandVariable(char * in)
{
  size_t buff = (strlen(in) + 2);
  char * input = malloc(sizeof(char) * buff);
  memset(input, '\0', sizeof(char) * buff);
  strcpy(input, in);

  int sizeInput = strlen(input);
  if(sizeInput > 0) {

    char * beg = NULL;
    char * end = NULL;
    char * output = NULL;

    for (int i = 0; i < sizeInput; i++)
    {
      // printf("[i]: %c, [i + 1]: %c   i: %d,  sizeInput: %d\n", input[i], input[i + 1], i, sizeInput);
      if(input[i] == '$' && input[i + 1] == '$') {

        char * replacement = getenv("PID");
        int sizeReplacement = strlen(replacement);


        output = (char *)malloc(sizeof(char) * (sizeInput + 1 + sizeReplacement));
        memset(output, '\0', (sizeof(char) * (sizeInput + 1 + sizeReplacement)));

        beg = substring(input, 0, i);
        strcat(output, beg);
        strcat(output, replacement);

        free(beg);
        beg=NULL;

        if(i < sizeInput - 2) {
          end = substring(input, i+2, sizeInput - i);
          strcat(output, end);
          free(end);
          end=NULL;
        }

        // printf("1. BEG: %s, PID: %s, END: %s, OUTPUT: %s\n", beg, replacement, end, output);

        // printf("EXAPNDED INPUT: %s\n", output);
        free(input);
        input = output;

        output = NULL;
        sizeInput = strlen(input);
      }
    }
  }
  return input;
}



/*
* Creates a new substring from a given string starting at position pos of len chars
* SOURCE: https://bit.ly/3IAdLPF, Date: 1/25/22, Adopted
*/
char * substring(char * str, int pos, int len)
{
  char * substring;
  int i;

  substring = (char *)malloc(sizeof(char) * (len + 1));
  memset(substring, '\0', sizeof(char) * (len + 1));

  if(substring == NULL) {
    exit(EXIT_FAILURE);
  }

  for ( i = 0; i < len; i++)
  {
    *(substring + i) = *((str + pos) + i);
  }

  return substring;
}



/*
* Checks for leading whitespace and trim if necessary
*/
void trimLeadingWhitespace(char * input)
{
  if(input[0] == ' ') {
    int i = 0;

    while (input[i] == ' ')
    {
      i++;
    }

    char * temp = substring(input, i, strlen(input) - i);
    memset(input, '\0', (MAX_LINE_LENGTH) * sizeof(char));
    strcat(input, temp);
    free(temp);      
  }
}

#endif