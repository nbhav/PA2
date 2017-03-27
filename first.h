/*
 * first.h
 *
 *  Created on: Feb 28, 2017
 *      Author: Neil
 */

#ifndef FIRST_H_
#define FIRST_H_

//maximum # of data & dict words
#define MaxWords 1000000
//maximum size of string
#define MaxLength 201
#define MaxWordBuffer MaxLength+1

void matchString(char *str);
void printResult();
void processFile(FILE *inDict,FILE *inData, FILE *outDict);
void addToList(char[],  char **, int[], int, int);
void addToListnoFreq(char item[],  char **list, int p, int n) ;
void printBuffer(int index, char *resultWord, int occurance, int superWord);

int genSortedArrayFmFile( FILE *in, char **wordList, char *word, int *frequency );
int genSortedArray( FILE *in, char **wordList, char *word);
int searchSuperWord(char word[], int strtIdx, char **dataPool, int* freqPool, int numPoolWords );
int getWord(FILE *, char[]);
int binarySearch(int, int, char[], char **);
int readData(FILE *in);
void readDict(FILE *in);

void merge(char** Left, char** Right, int n1 , int n2 , char** input);
void mergeSort(char** input, int numInputs);

//holds results in memory
typedef struct Result
{
 char  *rsltDict;
 int   rsltFreq;;
 int   rsltsuperCnt;
} Result;


#endif /* FIRST_H_ */
