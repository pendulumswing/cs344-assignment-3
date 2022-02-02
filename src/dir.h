#ifndef DIR_H
#define DIR_H

// #include "movie.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
#define FILEBASE "file"


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


#endif