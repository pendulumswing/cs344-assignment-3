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
  // int choice = 1;
  srandom(time(0));

  char input[MAX_LINE_LENGTH];
  CommandLine cl;
  // initCommandLine(&cl);



  do
  {
    shellPrompt();

    getInput(input, MAX_LINE_LENGTH);


    printf("You entered: %s\n\n", input);


    //-------------------------------------------------
    // Variable Exapansion of '$$'
    int length = strlen(input);
    int pid = 0;
    for (int i = 0; i < length; i++)
    {
      if(input[i] == '$' && input[i + 1] == '$') {
        pid = getpid();
        printf("PID: %d\\n", pid);
      }
    }

    // 1. Allocate memory for new string
    // 2. strcat last half of input string to new string
    // 3. convert PID to string
    // 4. Add PID as string to input at i with strcat
    // 5. Add remainder of string with strcat
    // 6. Free memory for temporary string
    //-------------------------------------------------



    // printf("Enter a number 1 or 2: ");
    
  //   // Get user choice
  //   scanf("%d%*c", &choice);   // Skip newline SOURCE: https://bit.ly/3A9rbPD, Date: 1/18/22, Adopted



  //   // Process choice
  //   switch (choice)
  //   {
  //     // 1. Select file to process
  //     case 1:
  //       {
  //         printf("You entered 1.\n\n");
  //       }
  //       break;

      
  //     // 2. Exit from the program
  //     case 2:
  //       break;
      
  //     // Handle incorrect choice
  //     default:
  //       {
  //         printf("You entered an incorrect choice. Try again.\n\n");
  //       }
  //       break;
  //   }

  // // } while (choice != 2);
  } while (strcmp(input, "exit") != 0);

  return EXIT_SUCCESS;
}
