# Assigment 2: Files & Directories

This program will:

- Reads directory entries
- Finds a file in the current directory based on user specified criteria
- Reads and processes the data in the chosen file
- Creates a directory
- Creates new files in the newly created directory and writes processed data to these files

### Compile:
Two methods to compile source code are provided below. The executable will be named `movies_by_year` and can be exectuted with `./movies_by_year`
<br>

##### Command line:
```
gcc --std=gnu99 -g -Wall -o movies_by_year main.c movie.c
```

##### Using included Makefile:
```
make         // build executable
make run     // build and run executable
make clean   // remove object files and executable
```