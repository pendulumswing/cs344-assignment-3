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
*   Date: 01/29/2022
*/

/*
*   Small shell program with limited functionality similar to BASH.
*   Compile the program as follows:
*       gcc --std=gnu99 -o smallsh smallsh.c
*/





int main(int argc, char *argv[])
{

  char input[MAX_LINE_LENGTH];
  int inputlen = 0;
  char cwd[MAX_LINE_LENGTH];
  memset(cwd, '\0', MAX_LINE_LENGTH * sizeof(char));


  // Get smallsh PID, as int and string
  int pid = getpid();
  char pidstr[12];
  sprintf(pidstr, "%d", pid);  // Convert pid to string
  setenv("PID", pidstr, 1);
  printf("Ennironment Variables:\n");
  printf("  PID: %s\n", getenv("PID"));
  printf("  HOME: %s\n", getenv("HOME"));

  // Foreground / Background Modes
  bool allowbg = true;            // Toggle with SIGTSTP  (CTRL-Z)
  char * fgonlymessage = "Entering foreground-only mode (& is now ignored)\n"; // count = 50 chars
  char * fgonlyexitmessage = "Exiting foreground-only mode\n"; // count = 30 chars


  // PIDS - to store all child process ids
  // Pids * fgpids = createPids();
  Pids * bgpids = createPids();

  int childStatus = 0;
  pid_t spawnPid;


  do
  {
    // PROMPT for shell
    shellPrompt();


    // GET user command string
    getInput(input, MAX_LINE_LENGTH);
    inputlen = strlen(input);


    // LEADING WHITESPACE - Check for leading whitespace and trim if necessary
    trimLeadingWhitespace(*&input);


    // COMMENTS & BLANK LINES - Reprompt if found
    if(input[0] == '#' || inputlen == 0 || hasSpacesOnly(input)) {
      // printf("IGNORING Comments, blank lines or only spaces\n");  // DEBUG
    } 
    else 
    {
      printf("  PPID: %s\n", pidstr);
      // printf("You entered: %s\n", input);

      // CREATE command and PARSE input string into args.
      // Identify file streams and if process is to run in background.
      Command * c = createCommand();
      c->parseInput(input, c);
      c->parseStreams(c);
      c->trimArgs(c);


      // CHECK for background processes (iterate over list)
      bgpids->check(bgpids);
      

      //----------------------------------------------------------------------------
      //
      //    BUILT-IN commands
      //
      //----------------------------------------------------------------------------
  
      //-------------------------------------------------
      //      EXIT
      // Terminate all child processes/jobs before parent
      // Then exit parent
      //-------------------------------------------------
      if(strcmp(c->name, "exit") == 0) {
        printf("BUILT-IN CMD: %s\n", c->name);

        // TODO: Terminate all child processes/jobs before parent
        bgpids->kill(bgpids);


      } 

      //-------------------------------------------------
      //      CD
      // Change working directory to given directory.
      // If no given directory, change to HOME directory.
      //-------------------------------------------------
      else if(strcmp(c->name, "cd") == 0) {
        // printf("BUILT-IN CMD: %s\n", c->name);  // DEBUG
        
        
        // DEBUG: Print CWD
        // getcwd(cwd, MAX_LINE_LENGTH);
        // printf("CWD: %s\n", cwd);


        if(c->numargs == 1) {
          chdir(getenv("HOME"));
        } else {
          chdir(c->args[1]);
        }

        // DEBUG: Print CWD
        // getcwd(cwd, MAX_LINE_LENGTH);
        // printf("CWD: %s\n", cwd);

      }

      //-------------------------------------------------
      //      STATUS
      // 1. Print exit status of terminating signal of the last foreground process
      //   a. If ran before any foreground command, then it should return exit status 0
      //   b. Built-in commands do not count as foreground processes (should be ignored)
      //-------------------------------------------------
      else if(strcmp(c->name, "status") == 0) {
        // printf("BUILT-IN CMD: %s\n", c->name);

        // TODO:
        // 1. Print exit status of terminating signal of the last foreground process
        //   a. If ran before any foreground command, then it should return exit status 0
        //   b. Built-in commands do not count as foreground processes (should be ignored)
        if(WIFEXITED(childStatus)){
          printf("  exit status %d\n", WEXITSTATUS(childStatus));
          fflush(stdout);
        } else{
          printf("  terminated by signal %d\n", WTERMSIG(childStatus));
          fflush(stdout);
        }


      } 
      
      //----------------------------------------------------------------------------
      //
      //    OTHER commands
      //
      //----------------------------------------------------------------------------
      else {

        // int   childStatus;
        // printf("  Parent's pid = %d\n", getpid());  // DEBUG
        // fflush(stdout);

        //-------------------------------------------------
        // FORK child process
        //-------------------------------------------------
        // TODO: Change this to use Pids array and store all pids
        spawnPid = fork();

        switch (spawnPid)
        {

          //-------------------------------------------------
          //    FAILED
          //-------------------------------------------------
          case -1:
                // Handle creation failure
                perror("fork() failed!");
                fflush(stdout);
                exit(1);
                break;


          //-------------------------------------------------
          //    CHILD process executes this
          //-------------------------------------------------
          case 0:
              {

                // // DEBUG
                // printf("  Child's pid = %d\n", getpid());
                // fflush(stdout);

                

                // I/O Redirection using dup2
                int result;

                //-------------------------------------------
                //    INPUT
                //-------------------------------------------
                if(c->hasInput) 
                {
                  // Open source file
                  c->fdin = open(c->finpath, O_RDONLY);
                  if(c->fdin == -1) {
                    perror("source open()");
                    exit(1);
                  }

                  printf("sourceFD == %d\n", c->fdin);


                  // Redirect stdin to source file
                  result = dup2(c->fdin, STDIN_FILENO);
                  if (result == -1) { 
                    perror("source dup2()"); 
                    exit(2); 
                  }
                }

                //-------------------------------------------
                //    OUTPUT
                //-------------------------------------------
                if(c->hasOutput) 
                {
                  // Open source file
                  c->fdout = open(c->foutpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                  if(c->fdout == -1) {
                    perror("target open()");
                    exit(1);
                  }

                  printf("targetFD == %d\n", c->fdout);


                  // Redirect stdout to target file
                  result = dup2(c->fdout, STDOUT_FILENO);
                  if (result == -1) { 
                    perror("target dup2()"); 
                    exit(2); 
                  }
                }

                // // Replace current program with provided one
                execvp(c->name, c->args);

                // Only returns if there is an error
                perror("execvp");
                exit(2);    // Be sure to exit Child process (not continue through rest of code)
                break;
              }
          

          //-------------------------------------------------
          //    PARENT process executes this
          //-------------------------------------------------
          default:
                // pid_t childPid = waitpid(spawnPid, &childStatus, c->isBg ? WNOHANG : 0);  // Wait on child to finish
                if(!c->isBg) {
                  spawnPid = waitpid(spawnPid, &childStatus, 0);  // Wait on child to finish
                } else {
                  printf("background pid is %d\n", spawnPid);
                  fflush(stdout);
                  bgpids->add(bgpids, spawnPid);
                  bgpids->print(bgpids);  // DEBUG
                };


                




                // printf("  WAITPID returned value %d\n", spawnPid);
                // fflush(stdout);

                // // Check STATUS of child process termination
                // if(WIFEXITED(childStatus)){
                //   printf("  CHILD %d exited normally with status %d\n", spawnPid, WEXITSTATUS(childStatus));
                //   fflush(stdout);
                // } else{
                //   printf("  CHILD %d exited abnormally due to signal %d\n", spawnPid, WTERMSIG(childStatus));
                //   fflush(stdout);
                // }
                // printf("PARENT is done waiting. The pid of child that terminated is %d\n", spawnPid);  // DEBUG
                // fflush(stdout);
                break;
        }

        // printf("The process with pid %d is returning from main\n", getpid());  // DEBUG
        // fflush(stdout);
      }

      // c->print(c);  // DEBUG
      c->free(c);

    }
  } while (strcmp(input, "exit") != 0);

  // fgpids->free(fgpids);
  bgpids->free(bgpids);

  return EXIT_SUCCESS;
}
