#include "movie.h"


/* 
*  Parse the current line which is comma delimited and create a
*  movie struct with the tokenized data in this line
*/
Movie *createMovie(char *currLine)
{
    Movie *currMovie = malloc(sizeof(Movie));

    // For use with strtok_r
    char *saveptr;
    char *saveptrLang;

    // The first token is the TITLE
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the YEAR
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);

    // The next token is the array of LANGAUGES
    token = strtok_r(NULL, ",", &saveptr);

    // 1. Strip first and last chars - SOURCE: https://bit.ly/32SSiCb, Type: adopted, Date: 1/5/22
    char *langstr = token + 1;
    langstr[strlen(langstr)-1]='\0';

    // 2. Set langauage array to 0
    for (int i = 0; i < 5; i++) {
      memset(currMovie->langs[i], 0, sizeof(char));
    }
    
    // 3. parse language string
    int count = 0;
    token = strtok_r(langstr, ";", &saveptrLang);
    strcpy(currMovie->langs[count], token);
    count++;

    token = strtok_r(NULL, ";", &saveptrLang);
    while(token != NULL) {
      strcpy(currMovie->langs[count], token); 
      count++;
      token = strtok_r(NULL, ";", &saveptrLang);
    }

    currMovie->numlangs = count;
    
    // The last token is the rating
    token = strtok_r(NULL, "\n", &saveptr);
    char *endstr;
    currMovie->rating = strtod(token, &endstr);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}
    

/*
* Initializes linked list of movie structs
*/
void initList(MovieList *list)
{
  list->front = NULL;
  list->back = NULL;
  list->size = 0;
}


/*
* Adds a given movie struct to the back of the movie linked list
*/
void addLinkToBack(MovieList *list, Movie *movie)
{
  movie->next = NULL;
  movie->prev = list->back;
  list->back->next = movie;
  list->back = movie;
  list->size++;
}


/*
* Return a linked list of movies by parsing data from
* each line of the specified file.
*/
MovieList *processFileToList(char *filePath)
{
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    int lineCount = 0;

    MovieList *list = (MovieList *)malloc(sizeof(MovieList));
    initList(list);


    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
      if(lineCount != 0) {
        // Get a new movie node corresponding to the current line
        Movie *newNode = createMovie(currLine);

        // Is this the first node in the linked list?
        if (list->front == NULL)
        {
          // This is the first node in the linked link
          // Set the head and the tail to this node
          list->front = newNode;
          list->back = newNode;
          newNode->next = NULL;
          newNode->prev = NULL;
          list->size++;
        }
        else
        {
          addLinkToBack(list, newNode);
        }
        lineCount++;
      } else {
        // Skip first line (headers, i.e. no data)
        lineCount++;
      }
    }
    free(currLine);
    fclose(movieFile);
    return list;
}



/*
* Print data for a given language array
*/
void printLanguages(char langs[MAX_NUM_LANG][MAX_LANG_LENGTH], int size) {
  for (int i = 0; i < size; i++)
  {
    if(langs[i][0] != 0) {
      printf("%s", langs[i]);

      if(i < (size - 1) && langs[i + 1][0] != 0) {
        printf(", ");
      }
    }
  }
  printf("\n");
}


/*
* Print year, rating, and title for a given movie
*/
void printMovie(Movie *amovie){
  printf("%d %.1f %s", 
              amovie->year,
              amovie->rating,
              amovie->title);
  // printLanguages(amovie->langs, 5);
}


/*
* Print year, rating, and title for a given movie
*/
void printMovieYearTitle(Movie *amovie){
  printf("%d %s", amovie->year, amovie->title);
}


/*
* Print the linked list of movies
*/
void printMovieList(Movie *list)
{
  while (list != NULL)
  {
      printMovie(list);
      list = list->next;
  }
}


/*
* Free memory for a linked list of movies
*/
void freeMovieList(Movie *list) {
  while (list != NULL)
  {
    Movie *temp = list;
    list = list->next;
    free(temp->title);
    temp->title = NULL;
    free(temp);
    temp = NULL;
  }
}


/*
* Free memory for a linked list of movies
*/
void freeMovieLinkedList(MovieList *list) {
  while (list->front != NULL)
  {
    Movie *temp = list->front;
    list->front = temp->next;
    free(temp->title);
    temp->title = NULL;
    free(temp);
    temp = NULL;
  }
  free(list);
}


/*
*  Count number of nodes in a movie linked list and return count.
*/
int getListLength(Movie *list) {
  int count = 0;
  Movie *current = list;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}


/*
*  Process movies list and return all movies for a given year
*/
int getMoviesByYear(MovieList *list, int year)
{
  Movie  *movie = list->front;
  int count = 0;

  do
  {
    if(movie->year == year)
    {
      printMovieTitle(movie);
      printf("\n");
      count++;
    }
    movie = movie->next;
  } while (movie->next != NULL);
  
  return count;
}


/*
*  Process movies list and return highest rated movie for each year
*/
int getHighestRatedMovies(MovieList *list)
{
  int count = 0;

  Movie  *highest = NULL;  // pointer to identify highest rating for a year
  Movie  *movie = NULL;  // pointer to front of list

  for (int year = 1900; year < 2022; year++)
  {
    highest = NULL;
    movie = list->front;

    // 2. iterate through list, if node has year, check against current highest node, if any
    do
    {
      if(movie->year == year)
      {
        if(highest == NULL)
        {
          highest = movie;
        } else if(movie->rating > highest->rating) {
          highest = movie;
        }
      }
      movie = movie->next;
    } while (movie->next != NULL);

    // 3. If node is not NULL, print value
    if(highest != NULL) {
      printMovie(highest);
      printf("\n");
      count++;
    }
  }
  return count;
}


/*
*  Print each movie that matches a given langauage. 
*  Return count for movies that match.
*/
int getMoviesByLanguage(MovieList *list, char *language)
{
  Movie  *movie = list->front;
  int count = 0;

  do
  {
    for (int i = 0; i < movie->numlangs; i++)
    {
      if(strcmp(language, movie->langs[i]) == 0)
      {
        printMovieYearTitle(movie);
        printf("\n");
        count++;
      }
    }
    movie = movie->next;
  } while (movie->next != NULL);
  
  return count;
}


/*
* print movie title
*/
void printMovieTitle(Movie *movie)
{
  printf("%s", movie->title);
}


/*
* Swap a movie in a given linked list with the movie that comes after it
*/
void swapNext(MovieList *list, Movie *movie)
{
  // Last in line
  if(movie->next == NULL || list->back == movie)
  {
    return;
  }

  // First in list
  if(list->front == movie)
  {
    list->front = movie->next;
    movie->next = list->front->next;
    list->front->next = movie;
    movie->prev = list->front;
  } 
  // Next is at back of list
  else if(list->back == movie->next){
    list->back->prev = movie->prev;
    list->back->next = movie;
    movie->next = NULL;
    movie->prev = list->back;
    list->back = movie;
  }
  // In the middle 
  else {
    Movie *b = movie->next;
    movie->next = b->next;
    b->prev = movie->prev;
    movie->prev = b;
  }
}
