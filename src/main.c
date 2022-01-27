#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movie.h"
#include "dir.h"
#include "shell.h"

#define PREFIX "movies_"
#define EXT_UP "CSV"
#define EXT_LOW "csv"


/*
*   Program Title: Assignment 3: smallsh
*   Author: Jason Brown
*   ONID: brownja4
*   Date: 01/24/2022
*/

/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o smallsh smallsh.c
*/
int main(int argc, char *argv[])
{

  char input[MAX_LINE_LENGTH];
  int inputlen = 0;
  // char variableExpandStr[MAX_LINE_LENGTH];

  // CommandLine cl;

  // Get smallsh PID, as int and string
  int pid = getpid();
  char pidstr[12];
  sprintf(pidstr, "%d", pid);  // Convert pid to string
  setenv("PID", pidstr, 1);

  printf("HOME: %s\n", getenv("HOME"));



  do
  {
    // Prompt for shell
    shellPrompt();

    // Get user command string
    getInput(input, MAX_LINE_LENGTH);
    inputlen = strlen(input);

    // Check for leading whitespace and trim if necessary
    trimLeadingWhitespace(*&input);


    // 2. Handle Comments & Blank Lines
    if(input[0] == '#' || inputlen == 0 || hasSpacesOnly(input)) {
      printf("IGNORING Comments, blank lines or only spaces\n");
    } 
    else 
    {
      printf("  PID: %s\n", pidstr);
      printf("You entered: %s\n", input);

      // Create command and parse input string into args
      // and identify file streams and if background process
      Command * c = createCommand();
      c->parseInput(input, c);
      c->parseStreams(c);
      c->trimArgs(c);


      // Check for built-in commands
      if(strcmp(c->name, "exit") == 0) {
        printf("BUILT-IN CMD: %s", c->name);

      }

      if(strcmp(c->name, "cd") == 0) {
        printf("BUILT-IN CMD: %s", c->name);

      }

      if(strcmp(c->name, "status") == 0) {
        printf("BUILT-IN CMD: %s", c->name);

      }


      // TODO: Create child process with command info...




      c->print(c);
      c->free(c);


    }
  } while (strcmp(input, "exit") != 0);

  // Exit Command
  


  return EXIT_SUCCESS;
}
