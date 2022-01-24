#ifndef DIR_H
#define DIR_H

#include "movie.h"
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>  // SOURCE: https://bit.ly/3tBzToE, Date: 1/17/22

#define MAX_FILENAME_SIZE 255
#define RANDSTRSIZE 6
#define RANDMAX 99999
#define RANDMIN 0
#define ONID "brownja4"
#define FILEBASE "movies"


typedef struct fileinfo {
  char name[MAX_FILENAME_SIZE];
  long long int size;
  long long unsigned int inode;
  long int fd;
} FileInfo;


void initFileInfo(FileInfo * file)
{
  memset(file->name, '\0', MAX_FILENAME_SIZE * sizeof(char));
  file->size = 0;
  file->inode = 0;
  file->fd = -1;
}


/*
* Prints directory name and inode for a given dirent struct.
*/
void printDirectoryInfo(struct dirent * aDir)
{
  // Print directory name and ino
  printf(" [aDir]\n"
          "  name: %s\n"
          "  inode: %llu\n", 
          aDir->d_name, 
          (long long unsigned int)aDir->d_ino
          );

}


/*
* Prints stats for a given path and buffer.
*/
void printStats(const char * pathname, struct stat * statbuf)
{
  // Get metadata for current entry
  stat(pathname, statbuf);
  printf(" [statbuf]\n" 
        "  mode: %hu\n"
        "  size: %lld BYTES\n"
        "  inode: %llu\n"
        "  uid: %d\n"
        "  gid: %d\n"
        "  atime: %ld SEC\n"
        "  mtime: %ld SEC\n\n", 
        statbuf->st_mode, 
        (long long int)statbuf->st_size, 
        (long long unsigned int) statbuf->st_ino, 
        statbuf->st_uid, 
        statbuf->st_gid,
        statbuf->st_mtime,
        statbuf->st_atime
        );
}

/*
* Returns non-zero integer (true) if regular file type.
*/
int isFile(const char * pathname, struct stat * statbuf)
{
  // Get metadata for current entry
  stat(pathname, statbuf);
  if(S_ISREG(statbuf->st_mode)) {
    printf("  FILE\n");
    return 1;
  } 
  return 0;
}

/*
* Returns non-zero integer (true) if regular file type.
*/
int isDirectory(const char * pathname, struct stat * statbuf)
{
  // Get metadata for current entry
  stat(pathname, statbuf);
  if(S_ISDIR(statbuf->st_mode)) {
    printf("  DIR\n");
    return 1;
  } 
  return 0;
}


/*
* Returns pointer to beginning of file extension for a given filename
* SOURCE: https://bit.ly/3GHhuKC, Date: 1/18/22, Adopted from source
*/
char * getFileExt(const char * filename)
{
  char * dot = strrchr(filename, '.');
  if(!dot || dot == filename) {
    return "";
  }
  return dot + 1;
}


/*
* Returns random integer between RANDMIN and RANDMAX, inclusive
* SOURCE: https://bit.ly/3IfAUXA, Date: 1/18/22, Adopted from source
*/
int getRandomNum()
{
  return (random() % (RANDMAX - RANDMIN + 1)) + RANDMIN;
}



/*
* Creates a new directory in the current working directory 
* with a random number suffix.
* In the format of: "ONID.movies.RANDOMNUMBER"
*/
void makeRandomDir(char * newDir)
{
  char rand[RANDSTRSIZE];
  memset(newDir, '\0', MAX_FILENAME_SIZE * sizeof(char));
  strcpy(newDir, ONID);
  strcat(newDir, ".");
  strcat(newDir, FILEBASE);
  strcat(newDir, ".");
  sprintf(rand, "%d", getRandomNum());  // SOURCE: https://bit.ly/3tU36vh, Date: 1/18/22, Adopted
  strcat(newDir, rand);
  mkdir(newDir, 00750);
}



/*
* Parses a MovieList and creates new .txt files for each year, in the format YYYY.txt
* where YYYY is the year. Creates files in a given directory path.
* Valid year range is MIN_YEAR to MAX_YEAR
* Each file will contain the title for each movie in that year, one per line
*/
void createFiles(MovieList * list, char * directoryPath)
{
  char * dirPath = directoryPath;  // Make a local copy of dirPath. The original 
                                   // was getting overwritten on flip for some reason
  char yearNum[5];
  int fd;
  char fileName[MAX_FILENAME_SIZE];
  char filePath[MAX_FILENAME_SIZE * 2];
  memset(fileName, '\0', MAX_FILENAME_SIZE * sizeof(char));
  memset(filePath, '\0', MAX_FILENAME_SIZE * 2 * sizeof(char));
  Movie  *movie = NULL;  // Pointer to fron of list

  // Iterate over list for each year
  // Create a file if first occurance of year, 
  // else open file and write to it
  for (int year = MIN_YEAR; year < MAX_YEAR; year++)
  {
    movie = list->front;

    do
    {
      if(movie->year == year)
      {
        if(filePath[0] == '\0')  // File not created yet
        {
          // Set file path based on year
          sprintf(yearNum, "%d", year);  // Convert year to string
          strcpy(filePath, dirPath);
          strcat(filePath, "/");
          strcat(filePath, yearNum);
          strcat(filePath, ".txt");

          // Create File and Open...
          fd = open(filePath, O_RDWR | O_CREAT | O_TRUNC, 0640);
          if (fd == -1){
            printf("open() failed on \"%s\"\n", filePath);
            perror("Error");
            exit(1);
          }
        } else {
          // ...Otherwise Open Existing file
          fd = open(filePath, O_RDWR | O_APPEND );
          if (fd == -1){
            printf("open() failed on \"%s\"\n", filePath);
            perror("Error");
            exit(1);
          }
        }

        // Write Title to file
        char message[strlen(movie->title) + 1];
        strcpy(message, movie->title);
        strcat(message, "\n");
        int howMany = write(fd, message, strlen(message));
        printf("wrote %d bytes to the file\n", howMany);

        close(fd);
      }

      // Go to next node in list
      movie = movie->next;

    } while (movie->next != NULL);


    // Reset filePath and fd
    fd = -1;
    memset(filePath, '\0', MAX_FILENAME_SIZE * 2 * sizeof(char));
  }
}


/*
* Processes CSV file into MovieList
* Creates a new directory, then creates files for each year within that directory,
* one for each year that occurs in the MovieList. The new files contain the titles
* of each movie for that year, one per line.
*/
void processFile(char * filename) {

  // 1. Process File
  printf("Now processing the chosen file named %s\n", filename);
  MovieList  *list = processFileToList(filename);


  // 2. Create Directory
  char dirName[MAX_FILENAME_SIZE];
  char dirPath[MAX_FILENAME_SIZE * 2];
  memset(dirPath, '\0', MAX_FILENAME_SIZE * 2 * sizeof(char));
  makeRandomDir(dirName);
  printf("Created directory with name %s\n", dirName);


  // 3. Create Directory Path
  strcat(dirPath, "./");
  strcat(dirPath, dirName);

  
  // 4. Write files to directory
  createFiles(list, dirPath);

  
  // 5. Free list memory
  freeMovieLinkedList(list);
}


#endif