#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movie.h"
#include "dir.h"

#define PREFIX "movies_"
#define EXT_UP "CSV"
#define EXT_LOW "csv"


/*
*   Program Title: Assignment 2: Files & Directories
*   Author: Jason Brown
*   ONID: brownja4
*   Date: 01/22/2022
*/

/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies movies.c
*/
int main(int argc, char *argv[])
{
  int choice = 1;
  srandom(time(0));

  do
  {
    // Provide menu of choices to user
    printf("1. Select file to process\n"
          "2. Exit the program\n\n"
          "Enter a choice from 1 or 2:  "
    );
    
    // Get user choice
    scanf("%d%*c", &choice);   // Skip newline SOURCE: https://bit.ly/3A9rbPD, Date: 1/18/22, Adopted

    // Process choice
    switch (choice)
    {
      // 1. Select file to process
      case 1:
        {
          int subchoice = -1;

          do
          {

            // Open Directory
            DIR *currDir = opendir(".");
            struct dirent *aDir;

            // Create structs to store stats and file info
            struct stat dirStat;
            FileInfo file;
            initFileInfo(&file);

            // Provide submenu of choices to user
            printf(
              "\nWhich file do you want to process?\n"
              "Enter 1 to pick the largest file\n"
              "Enter 2 to pick the smallest file\n"
              "Enter 3 to specify the name of a file\n\n"
              "Enter a choice from 1 to 3: "
            );

            // Get user input
            scanf("%d%*c", &subchoice);

            // Handle choice
            switch (subchoice)
            {
              // 1. Pick the largest file
              case 1:
                {

                  // Go through all entries
                  while((aDir = readdir(currDir)) != NULL)
                  {

                    // // Check for match
                    if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {

                      // Grab extension from file name
                      char * extension = getFileExt(aDir->d_name);

                      // Check if extension is indeed a 'csv' or 'CSV' type
                      if(strncmp(EXT_LOW, extension, strlen(EXT_LOW)) == 0 || strncmp(EXT_UP, extension, strlen(EXT_UP)) == 0) {

                        stat(aDir->d_name, &dirStat);

                        if(file.name[0] == '\0') {
                          strcpy(file.name, aDir->d_name);
                          file.size = dirStat.st_size;
                        } else if (dirStat.st_size >= file.size) {
                          strcpy(file.name, aDir->d_name);
                          file.size = dirStat.st_size;
                        }
                      }
                    }
                  }
                }
                break;

              
              // 2. Pick the smallest file
              case 2:
                {

                  // Go through all entries
                  while((aDir = readdir(currDir)) != NULL)
                  {

                    // // Check for match
                    if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {

                      // Grab extension from file name
                      char * extension = getFileExt(aDir->d_name);

                      // Check if extension is indeed a 'csv' or 'CSV' type
                      if(strncmp(EXT_LOW, extension, strlen(EXT_LOW)) == 0 || strncmp(EXT_UP, extension, strlen(EXT_UP)) == 0) {

                        stat(aDir->d_name, &dirStat);

                        if(file.name[0] == '\0') {
                          strcpy(file.name, aDir->d_name);
                          file.size = dirStat.st_size;
                        } else if (dirStat.st_size <= file.size) {
                          strcpy(file.name, aDir->d_name);
                          file.size = dirStat.st_size;
                        }
                      }
                    }
                  }
                }
                break;
              
              // 2. Specify the name of a file
              case 3:
                {
                  char requestedFile[MAX_FILENAME_SIZE];
                  memset(requestedFile, '\0', MAX_FILENAME_SIZE * sizeof(char));
                  printf("Enter the complete file name: ");
                  fgets(requestedFile, MAX_FILENAME_SIZE, stdin);
                  requestedFile[strcspn(requestedFile, "\n")] = '\0';  // Remove newline SOURCE: https://bit.ly/3fyCbwz, Date 1/18/22, Adopted

                  // Go through all entries
                  while((aDir = readdir(currDir)) != NULL)
                  {
                  
                    // Check for match
                    if(strncmp(requestedFile, aDir->d_name, strlen(aDir->d_name)) == 0 
                      && strncmp(requestedFile, aDir->d_name, strlen(requestedFile)) == 0) {

                      stat(aDir->d_name, &dirStat);
                      strcpy(file.name, requestedFile);
                      file.size = dirStat.st_size;
                    }
                  }

                  if(file.name[0] == '\0') {
                    printf("The file %s was not found. Try again\n\n", requestedFile);
                    subchoice = 4;   // To keep in loop of subchoices
                  }
                }
                break;

              // Handle incorrect choice
              default:
                {
                  printf("You entered an incorrect choice. Try again.\n\n");
                }
                break;
            }


            // Process File
            if(file.name[0] != '\0') {
              processFile(file.name);
            }

            closedir(currDir);

          } while (subchoice < 1 || subchoice > 3);
        }
        break;

      
      // 2. Exit from the program
      case 2:
        break;
      
      // Handle incorrect choice
      default:
        {
          printf("You entered an incorrect choice. Try again.\n\n");
        }
        break;
    }

  } while (choice != 2);

  return EXIT_SUCCESS;
}
