Assigment 2: Files & Directories

This program will:

1. Provide a prompt for running commands
2. Handle blank lines and comments, which are lines beginning with the `#` character
3. Provide expansion for the variable $$
4. Execute 3 commands `exit`, `cd`, and `status` via code built into the shell
5. Execute other commands by creating new processes using a function from the `exec` family of functions
6. Support input and output redirection
7. Support running commands in foreground and background processes
8. Implement custom handlers for 2 signals, SIGINT and SIGTSTP


Compile:
Two methods to compile source code are provided below. 
The executable will be named `smallsh` and can be exectuted with `./smallsh`


Command line:
gcc --std=gnu99 -g -Wall -o smallsh main.c


Using included Makefile:
make         // build executable
make run     // build and run executable
make clean   // remove object files and executable