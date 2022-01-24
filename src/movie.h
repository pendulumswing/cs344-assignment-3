#ifndef MOVIE_H
#define MOVIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_LANG 5
#define MAX_LANG_LENGTH 21
#define MIN_YEAR 1900
#define MAX_YEAR 2022

/* 
* movie struct
* - Cannot assume max length of title (dynamic)
* - Year is positive integer between 1900 and 2100
* - Languages is one or more strings values enclosed by brackets
*   Max langurages is 5 and max length of string is 20; no particular case
* - Rating between 1 and 10 inclusive, integer or double
*/
typedef struct movie
{
  char *title;
  int year;
  char langs[MAX_NUM_LANG][MAX_LANG_LENGTH];
  int numlangs;
  double rating;
  struct movie *next;
  struct movie *prev;
} Movie;

/* 
* list struct
* Doubly Linked list of movie structs
*/
typedef struct list
{
  int size;
  Movie *front;
  Movie *back;
} MovieList;


Movie *createMovie(char *currLine);
void initList(MovieList *list);
MovieList *processFileToList(char *filePath);

void printLanguages(char langs[MAX_NUM_LANG][MAX_LANG_LENGTH], int size);
void printMovie(Movie *amovie);
void printMovieYearTitle(Movie *amovie);
void printMovieList(Movie *list);
void printMovieTitle(Movie *movie);

void freeMovieList(Movie *list);
void freeMovieLinkedList(MovieList *list);
int getListLength(Movie *list);

// 1. Process movies list and return all movies for a given year
int getMoviesByYear(MovieList *list, int year);
// 2. Process movies list and return highest rated movie for each year
int getHighestRatedMovies(MovieList *list);
// 3. Process movies list and return all movies for a given langauge
int getMoviesByLanguage(MovieList *list, char *language);

void swapNext(MovieList *list, Movie *movie);
void addLinkToBack(MovieList *list, Movie *movie);

#endif