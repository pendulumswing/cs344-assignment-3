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
  int choice = 1;
  srandom(time(0));

  char input[MAX_LINE_LENGTH];

  do
  {
    // Provide menu of choices to user
    shellPrompt();

    getInput(input, MAX_LINE_LENGTH);


    printf("You entered: %s\n\n", input);

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
  } while (strcmp(input, "2") != 0);

  return EXIT_SUCCESS;
}
