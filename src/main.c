#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dir.h"
#include "shell.h"

/*
*   Program Title: Assignment 3: smallsh
*   Author: Jason Brown
*   ONID: brownja4
*   Date: 01/30/2022
*/

/*
*   Small shell program similar to BASH, with limited functionality.

*   Compile the program as follows:
*       gcc --std=gnu99 -o smallsh smallsh.c
*
*   This program will:   
*   1. Provide a prompt for running commands
*   2. Handle blank lines and comments, which are lines beginning with the `#` character
*   3. Provide expansion for the variable $$
*   4. Execute 3 commands `exit`, `cd`, and `status` via code built into the shell
*   5. Execute other commands by creating new processes using a function from the `exec` family of functions
*   6. Support input and output redirection
*   7. Support running commands in foreground and background processes
*   8. Implement custom handlers for 2 signals, SIGINT and SIGTSTP
*/



// GLOBAL Variables
static bool allowbg = true;


/*
* Signal handler for SIGTSTP (Control-Z)
* Toggles allowbg bool and prints message to stdout.
* Use "kill -SIGTSTP $$" at prompt to test (Using Control-Z
* will kill the smallsh process as well).
*/
void handle_SIGTSTP(int signo)
{
  char * fgonlymessage = "\nEntering foreground-only mode (& is now ignored)\n"; // count = 51 chars
  char * fgonlyexitmessage = "\nExiting foreground-only mode\n"; // count = 31 chars
  if(allowbg) {
    write(STDOUT_FILENO, fgonlymessage, 51);
  } else {
    write(STDOUT_FILENO, fgonlyexitmessage, 31);
  }
  allowbg = !allowbg;
};




/*
* MAIN Functino for program entry point.
*/
int main(int argc, char *argv[])
{

  char input[MAX_LINE_LENGTH];
  int inputlen = 0;
  char cwd[MAX_LINE_LENGTH];
  memset(cwd, '\0', MAX_LINE_LENGTH * sizeof(char));


  // Initialize sigaction structs
  struct sigaction SIGTSTP_action = {0}, ignore_action = {0};

  // Fill out the SIGINT_action struct
  // Register handle_SIGINT as the signal handler
  SIGTSTP_action.sa_handler = handle_SIGTSTP;
  // Block all catchable signals while handle_SIGINT is running
  sigfillset(&SIGTSTP_action.sa_mask);
  // No flags set
  SIGTSTP_action.sa_flags = 0;
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);


  // The ignore_action struct as SIG_IGN as its signal handler
  ignore_action.sa_handler = SIG_IGN;

  // Register the ignore_action as the handler for SIGTERM, SIGHUP, SIGQUIT.
  // So all three of these signals will be ignored.
  sigaction(SIGTERM, &ignore_action, NULL);
  sigaction(SIGHUP, &ignore_action, NULL);
  sigaction(SIGQUIT, &ignore_action, NULL);


  // PARENT PID - as int and string
  int pid = getpid();
  char pidstr[12];
  sprintf(pidstr, "%d", pid);
  setenv("PID", pidstr, 1);


  // // DEBUG
  // printf("Ennironment Variables:\n");
  // printf("  PID: %s\n", getenv("PID"));
  // printf("  HOME: %s\n", getenv("HOME"));


  // PIDS - to store all child process ids
  // Pids * fgpids = createPids();
  Pids * bgpids = createPids();

  int childStatus = 0;
  pid_t spawnPid;


  do
  {
    // CHECK for background processes (iterate over list)
    bgpids->check(bgpids);


    // PROMPT
    shellPrompt();


    // GET user command string
    getInput(input, MAX_LINE_LENGTH);
    inputlen = strlen(input);


    // TRIM LEADING WHITESPACE - Check for leading whitespace and trim if necessary
    trimLeadingWhitespace(*&input);


    // COMMENTS & BLANK LINES - Reprompt if found
    if(input[0] == '#' || inputlen == 0 || hasSpacesOnly(input)) {
      // printf("IGNORING Comments, blank lines or only spaces\n");  // DEBUG
    } 
    else 
    {
      // printf("  PPID: %s\n", pidstr);  // DEBUG
      // printf("You entered: %s\n", input);

      // CREATE command and PARSE input string into args.
      Command * c = createCommand();
      c->parseInput(input, c);
      c->parseStreams(c);
      c->trimArgs(c);


      

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

        // Terminate all child processes/jobs before parent
        bgpids->kill(bgpids);

      } 

      //-------------------------------------------------
      //      CD
      // Change working directory to given directory.
      // If no given directory, change to HOME directory.
      //-------------------------------------------------
      else if(strcmp(c->name, "cd") == 0) {
        
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

        //-------------------------------------------------
        // FORK child process
        //-------------------------------------------------
        spawnPid = fork();

        switch (spawnPid)
        {

          //-------------------------------------------------
          //    FAILED
          //-------------------------------------------------
          case -1:
          
                perror("fork() failed!");
                fflush(stdout);
                exit(1);
                break;


          //-------------------------------------------------
          //    CHILD Process
          //-------------------------------------------------
          case 0:
              {
                int result;

                //-------------------------------------------
                //    INPUT Stream
                //-------------------------------------------
                if(c->hasInput) 
                {
                  // Open source file
                  c->fdin = open(c->finpath, O_RDONLY);
                  if(c->fdin == -1) {
                    perror("source open()");
                    exit(1);
                  }

                  // printf("sourceFD == %d\n", c->fdin);  // DEBUG


                  // Redirect stdin to source file
                  result = dup2(c->fdin, STDIN_FILENO);
                  if (result == -1) { 
                    perror("source dup2()"); 
                    exit(2); 
                  }
                }

                //-------------------------------------------
                //    OUTPUT Stream
                //-------------------------------------------
                if(c->hasOutput) 
                {
                  // Open source file
                  c->fdout = open(c->foutpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                  if(c->fdout == -1) {
                    perror("target open()");
                    exit(1);
                  }

                  // printf("targetFD == %d\n", c->fdout);  // DEBUG


                  // Redirect stdout to target file
                  result = dup2(c->fdout, STDOUT_FILENO);
                  if (result == -1) { 
                    perror("target dup2()"); 
                    exit(2); 
                  }
                }

                //-------------------------------------------
                //    EXECUTE - Replace current program with provided one
                //-------------------------------------------
                execvp(c->name, c->args);

                
                perror("execvp");  // Only returns to this code if there is an error
                _exit(2);           // Exit Child process (i.e. - don't continue through rest of code)
                break;
              }
          

          //-------------------------------------------------
          //    PARENT Process
          //-------------------------------------------------
          default:
                // pid_t childPid = waitpid(spawnPid, &childStatus, c->isBg ? WNOHANG : 0);  // Wait on child to finish

                // BACKGROUND Process
                if(c->isBg && allowbg) {
                  printf("background pid is %d\n", spawnPid);
                  fflush(stdout);
                  bgpids->add(bgpids, spawnPid);
                  // bgpids->print(bgpids);  // DEBUG
                } 
                
                // FOREGROUND Process
                else {
                  spawnPid = waitpid(spawnPid, &childStatus, 0);  // Wait on child to finish
                };
                break;
        }
      }
      c->free(c);

    }
  } while (strcmp(input, "exit") != 0);

  // fgpids->free(fgpids);
  bgpids->free(bgpids);

  return EXIT_SUCCESS;
}
