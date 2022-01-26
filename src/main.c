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



  do
  {
    // Prompt for shell
    shellPrompt();

    // Get user command string
    getInput(input, MAX_LINE_LENGTH);
    inputlen = strlen(input);


    // 2. Handle Comments & Blank Lines
    if(input[0] == '#' || inputlen == 0 || hasSpacesOnly(input)) {
      printf("IGNORING Comments, blank lines or only spaces\n");
    } 
    else 
    {
      printf("You entered: %s\n", input);

      TODO:
      // 1. Generate Command structs while not '<', '>', or '&'



      Command * command = createCommand(input);

      printf("  CMD: %s\n", command->name);
      printf("  ARGS: ");
      fflush(stdout);
      for (int i = 0; i < command->numargs; i++)
      {
        printf("%s ", command->args[i]);
      }
      printf("\n");
      fflush(stdout);

      command->free(command);

    }
  } while (strcmp(input, "exit") != 0);

  // Exit Command
  


  return EXIT_SUCCESS;
}
