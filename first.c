#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "first.h"

//holds words from dataFile while processing
char  wordData[MaxWordBuffer];
int numDataWords = 0;
int *freqData; //holds freq. of duplicate words within dataFile
char **dataList;

//holds words from dictionaryFile while processing
char  wordDict[MaxWordBuffer];
int numDictWords = 0;
int *freqDict;
char **dictList;


int superCnt = 0; //# of superwords
int searchIdx = 0;
int dictCnt = 0;


Result *outResult;

FILE *mapFile, *dictFile, *dataFile, *outFile;

/*
 *
 * Main Method starts here:
 * initializes dictionary array name, data array name and outputfile name
 * exit(1) if there is any errors with mapFile
 * reads input from mapFile and creates filepointers to the dictionary and datafiles respectively
 * if dictionaryFile is null it will create a empty outputfile and continue
 * Calls processFile on dictionary, data, and output files
 * returns 0 if there are no errors
 */
int main (int argc, char **argv){
  char DICTNAME[30];//dict_i
  char DATANAME[30];//data_i
  char OUTNAME[30];//out_i
  int outputFileNum = 1;
  mapFile = fopen(argv[1], "r");

  if (argc != 2 || mapFile == NULL) {
      printf("invalid input\n");
        exit(1);
   }

  while((fscanf (mapFile, "%s %s", DICTNAME, DATANAME))!=EOF){
      printf("%s %s \n", DICTNAME, DATANAME);
      dictFile = fopen(DICTNAME, "r");
      dataFile  = fopen (DATANAME, "r");

      if(dictFile == NULL){
         sprintf(OUTNAME,"out%d.txt", outputFileNum);
         outFile = fopen(OUTNAME, "w");
         fclose(outFile);
         outputFileNum++;
         continue;
       }else{
        //dictionaryFile is not NULL
		 superCnt = 0;
		 searchIdx = 0;
		 dictCnt = 0;
		 //creates appropriate output file name
         sprintf(OUTNAME,"out%d.txt", outputFileNum);
         outFile = fopen( OUTNAME, "w");
         processFile(dictFile,dataFile, outFile);
         fclose(dictFile);
         fclose (dataFile);
         fclose (outFile);
         outputFileNum++;
    }

    }
   fclose(mapFile);
   return 0;
}//end main

/*
 * processFile
 * input: File pointer to dictionary, dataFile, and outFile
 * populates array for dictionary and data files and intializes freq array,
 * then outputs resulting matches and superwords to outputFile
 * output: nothing
 *
 *
 */
void processFile(FILE *inDict,FILE *inData, FILE *outDict){
	//creates pointers to integer to hold freq. of dup. words
   freqData = (int *)calloc(MaxWords, sizeof(int));
   //creates 2-D array for dataFile words
   dataList =  (char**)calloc(MaxWords, sizeof(char *));
   //populates data array and returns number of data words
   numDataWords = readData(inData);

   freqDict = (int *)calloc(MaxWords, sizeof(int));
   //creates list 2-D array for diectiony words
   dictList =  (char**)calloc(MaxWords, sizeof(char *));
   //populates dictionary array
   readDict(inDict);
   if(numDictWords == 0){return;}
   outResult = (Result *)calloc(MaxWords, sizeof(Result));
    if((numDictWords != 0 ) && (numDataWords != 0)){
	     for ( dictCnt = 0; dictCnt < numDictWords; dictCnt++){
   	    // get first word (string) from dictionary wordDict[dictCnt]
		      if (searchIdx < numDataWords -1 ) {
			       searchIdx = binarySearch (searchIdx, numDataWords-1, dictList[dictCnt], dataList);
		  	     if(searchIdx <= numDataWords -1){
	 			       matchString(dictList[dictCnt]);
			      }else{
				       printBuffer( dictCnt, dictList[dictCnt],0, 0 );
			       }
		      	}else{
		          printBuffer( dictCnt, dictList[dictCnt],0, 0 );
	    	  }
	     }
    }else{
		  if(numDataWords == 0){
		        // no dataWords
			  if (numDictWords == 0 ){
				  	  return;
			  }else{
				 for(int dictCnt = 0; dictCnt < numDictWords; dictCnt++){
					 //generating buffer of only dictwords
					 printBuffer(dictCnt, dictList[dictCnt], 0, 0);
				 }
			  }
		   }//end if
    }//end else
    printResult();
    free(freqData);
    for(int i=0; i <= numDataWords-1; i++) {
        free(dataList[i]);
    }
    free(dataList);
    free(freqDict);
    for(int i=0; i <= numDictWords-1; i++){
        free(dictList[i]);
    }
    free(dictList);
    free(outResult);
} // end processFile


/*
 * readData
 * input: File pointer to dataFile
 * get value from genSrotedArrayFmFile(# of Data Words)
 * output: 0 if NULL or # of dataWords from file
 *
 */
int readData(FILE *in){
  if(in == NULL){
    return 0;
  }
  int value;
  value = genSortedArrayFmFile( in, dataList, wordData, freqData );
  return(value);
}//end readData



/*
 * readDict
 * input: File pointer to dictFile
 * get value from genSortedArrayFmFile(# of Data Words)
 * output: void
 *
 */
void readDict(FILE *in){
	numDictWords = genSortedArray( in, dictList, wordDict);
}//end readDict



/**
 *
 * getWord inputs a file pointer(in) and a string array
 * The first loop removes any garbage input
 * Second loop adds chars' until it finds a non-alpha
 * getWord returns 1 if successful string array is created
 * getWord returns 0 if no word are left to process
 *
 */
int getWord(FILE * in, char str[]) {
    char ch;
    int n = 0;
    while (!isalpha(ch = getc(in)) && ch != EOF);
    if (ch == EOF){
    	return 0;
    }
    str[n++] = tolower(ch);
    while (isalpha(ch = getc(in)) && ch != EOF){
        if (n < MaxLength){
        	str[n++] = tolower(ch);
        }
    }
    str[n] = '\0';
    return 1;
} // end getWord



/**
 * addToList
 * input: getWord(word), data or dictList, freq. array, position to add at, total number of words
 * moves everything down until you hit position that binarySearch indicated (if more than one word), then add the word
 * output: nothing
 */
void addToList(char item[],  char **list,int freq[], int p, int n) {
	//adds item in position list[p]; sets freq[p] to 1
	//shifts list[n] down to list[p]
    for (int h = n; h > p; h--) {
        list[h] = list[h-1];
        freq[h] = freq[h-1];
    }
    //adding item
    list[p] = (char *)malloc((strlen(item) + 1) * sizeof(char));
    strcpy(list[p], item);
    freq[p] = 1;
}//end addTolist

/*
 * same as above just no freq.
 *
 */
void addToListnoFreq(char item[],  char **list, int p, int n){
	//adds item in position list[p]; sets freq[p] to 1
	//shifts list[n] down to list[p]
    for (int h = n; h > p; h--) {
        list[h] = list[h-1];
    }
    //adding item
    list[p] = (char *)malloc((strlen(item) + 1) * sizeof(char));
    strcpy(list[p], item);
}//end addTolistnoFreq


/*
 * binarySearch
 * input: lo, numWords-1 , word to search for, 2D array
 * output: location at which either duplicate or location at which to add
 */
int binarySearch(int lo, int hi, char key[],  char **list) {
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        int cmp = strcmp(key, list[mid]);
        if (cmp == 0){
        	//match!
        	return mid;
        }
        if(cmp < 0){
        	//string was less than mid
        	hi = mid - 1;
        }else{
        	//string was greater than mid
        	lo = mid + 1;
        }
    }
    return lo; //not found; should be inserted in location lo
} //end binarySearch


/*
 * genSortedArrayFmFile
 *inputs:File pointer, 2D array of words, input word and freq. array
 * sorts all the words in the given file to the approriate position
 *output: # of words that are currently in the 2-D array
 *
 */

int genSortedArrayFmFile( FILE *in, char **wordList, char *word, int *frequency){
  int numWords = 0;
  //if word post fixing isn't empty
  if(getWord(in, word) != 0){
      addToList(word,  wordList, frequency, 0, 0);
      ++numWords;
  }else{
	  //nothing has changed
	   return (numWords);
  }
  //runs until end of file
  while (getWord(in, word) != 0){
	 //gets location of which to be added or location of duplicate word
	 int loc = binarySearch (0, numWords-1, word,wordList);
     if ((wordList[loc] != NULL) && (strcmp(word, wordList[loc]) == 0)){
         ++frequency[loc]; //word found
     }else{
    	 //entering new word
    	 addToList(word,  wordList, frequency, loc, numWords);
       	  ++numWords;
    }
  }
  return (numWords);
}//end genSortedArrayFmFile

/*
 * Same thing as top just no freq array changing since no need for dictionary file
 *
 */
int genSortedArray( FILE *in, char **wordList, char *word){
  int numWords = 0;
  //if word post fixing isn't empty
  if(getWord(in, word) != 0){
      addToListnoFreq(word,  wordList, 0, 0);
      ++numWords;
  }else{
	  //nothing has changed
	   return (numWords);
  }
  //runs until end of file
  while (getWord(in, word) != 0){
	 //gets location of which to be added or location of duplicate word
	 int loc = binarySearch (0, numWords-1, word,wordList);
     if ((wordList[loc] != NULL) && (strcmp(word, wordList[loc]) == 0)){
    	 continue;
     }else{
    	 //entering new word
    	// addToListnoFreq(word,  wordList, loc, numWords);
           wordList[numWords] = word;
           mergeSort(wordList,numWords);
       	  ++numWords;
    }
  }
  return (numWords);
}//end genSortedArray

/**
 * searchSuperWord
 * input: word from dictionary array, start search index, list of data words, freq. of words, # of data words
 * takes input words and looks for superwords
 * output: # of superwords for given dictionary word
 *
 */
int searchSuperWord(char *word, int strtIdx, char **dataPool, int* freqPool, int numPoolWords ){
	int prefixCnt = 0, rslt;  // superWordCnt
	int lenstr = strlen(word);
	//while startidx is not greather than #ofdata words and
	while((strtIdx <= numPoolWords) && ((rslt = strncmp(word, dataPool[strtIdx], lenstr))==0)){
		if (rslt == 0){
			prefixCnt = prefixCnt + freqPool[strtIdx];
	     }
	     strtIdx++;
	  }
    return(prefixCnt);
}//end searchSuperWord



/**
 * printBuffer
 * input: index of word, word, # of occurrences, and number of superwords
 *  updates outResult structure for given word to given counts
 * output: nothing
 */
void printBuffer(int index, char *resultWord, int occurrences, int superWord){
     outResult[index].rsltDict = resultWord;
     outResult[index].rsltFreq = occurrences;
     outResult[index].rsltsuperCnt = superWord;
}//end printBuffer


/**
 * matchString
 * input: string
 * checks to see if there is a duplicate word and updates the count,
 * then checks to see if there are superwords for the given word
 * output:void
 */
void matchString(char* str){
	//check if word exists
   	if (strcmp(str, dataList[searchIdx]) == 0){
   		//start search index after increasing index by 1
   		superCnt = searchSuperWord(str, searchIdx + 1, dataList, freqData, numDataWords -1);
		printBuffer(dictCnt, dictList[dictCnt], freqData[searchIdx], superCnt);
    }else{
       //word not found
    	//start searching at that at that index
    	superCnt = searchSuperWord(str, searchIdx   , dataList, freqData, numDataWords -1);
    	printBuffer(dictCnt, dictList[dictCnt], 0, superCnt);
	}
}//end matchString


/**
 * printResult
 * input:nothing
 * prints all words and associated counts to output file
 * output:nothing
 */
void printResult(){
    for (int h = 0; h < numDictWords; h++){
        fprintf(outFile, "%s %d %d\n", outResult[h].rsltDict, outResult[h].rsltFreq, outResult[h].rsltsuperCnt);
    }
}//end printResult



void merge(char** Left, char** Right, int n1 , int n2 , char** input){
    int i, j , k;
    i = 0;
    j = 0;
    k = 0;


    while(i < n1 && j < n2){
    	if(strcmp(Left[i],Right[i]) <= 0){
    		strcpy(input[k],Left[i]);
    		i++;
    		k++;
    	}else{
    		strcpy(input[k],Right[j]);
    		i++;
    		k++;
    	}
    }
    while(i < n1){
    	strcpy(input[k],Left[i]);
    	i++;
    	k++;
    }
    while(j<n2){
    	strcpy(input[k],Right[j]);
    	j++;
    	k++;
    }

}

void mergeSort(char** input, int numInputs){
    int i, n1, n2;
    char** Left;
	char** Right;
    if(numInputs < 2){
    	return;
    }
    n1 = numInputs /2;
    n2 = numInputs - n1;
    Left = (char **)calloc(n1,sizeof(char));
    Right = (char **)calloc(n2,sizeof(char));

    for(i = 0; i < n1; i++){
    	strcpy(Left[i],input[i]);
    }
    for(i = 0; i < n2; i++){
    	strcpy(Right[i],input[i + n1]);
    }
    mergeSort(Left, n1);
    mergeSort(Right,n2);

    merge(Left,Right,n1,n2,input);
    free(Left);
    free(Right);

}



