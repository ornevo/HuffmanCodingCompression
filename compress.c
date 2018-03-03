#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "headers.h"

int main(int argc, char *argv[]){
	#ifdef PROGRESS_BAR_ENABLE
		precentagesNum = currPrecantageProg = 0;
		toPrint = (char*)malloc((outOf + 3) * sizeof(char));
		toPrint[outOf + 2] = '\0';
	#endif

	if(argc != 3){
		fprintf(stderr, "ERROR: You must pass two arguments to the program: -c/d, and a file name.\n");
		exit(WRONG_NUMBER_OF_PARAMETERS);
	} else if (argv[1][1] == 'c')
	  compress(argv[2]);
	else if(argv[1][1] == 'd')
		decompress(argv[2]);
	else{
		fprintf(stderr, "%s is not a valid option.\n", argv[1]);
		exit(NOT_VALID_OPTION_EXIT_CODE);
	}
	puts("Finished Successfully!");

	#ifdef PROGRESS_BAR_ENABLE
		free(toPrint);
	#endif
	return EXIT_SUCCESS;
}

void compress(char *source){
  char *destName = getDestName(source), curr, *cPath, currToInsert = 0;
  unsigned char lastByteFill = 0;
  FILE *src = fopen(source, "rt"), *dest = fopen(destName, "wb"); // open the files
  TNode *arr;
  TNode *huffTree;
  short freqMapSize;
  long fileSize;

  /* files opening errors handeling */
  if(!src || !dest){
	  if(!src){
			fprintf(stderr, "Error opening %s: ", source);
			remove(destName);
		}
	  else
			fprintf(stderr, "Error writing to %s: ", destName);

	  free(destName);
	  fcloseall();

	  fprintf(stderr, "%s\n", strerror(errno));
	  exit(FILE_ERR_EXIT_CODE);
  } else if(strcmp(getFileExten(source), "txt")){ // file format check
		fprintf(stderr, "ERROR: The file %s isn't a plain text file, and hence can't be compressed by this program.\n", source);
		remove(destName);

		exit(FILE_ERR_WRONG_FORMAT);
	}
	/* check the file isn't empty */
	if(fgetc(src) == EOF){ // if there is no characters in the file.
		fprintf(stderr, "ERROR: The file you tried to compress was empty.\n");
		remove(destName);

		exit(EMPTY_FILE_ERR);
	}
	rewind(src);

  #ifdef PROGRESS_BAR_ENABLE
	  // determine file size, for progress bar
	  fseek(src, 0, SEEK_END);
	  precentageSize = ftell(src) / outOf;
	  rewind(src);
  #endif

  arr = fileToFreqMap(src); // generate frequencies map from the source file.
  printf("Building Huffman tree from frequencies...");
  huffTree = buildTreeFromArr( cloneMap(arr) ); // build a Huffman tree from the map
  puts(" Done.");

  #ifdef DEBUG
    printf("The map: " );
    printArr(arr);
  #endif
  freqMapSize = ((short)getMapSize(arr)) * 5; // the *5 is because every char and freq takes 5 bytes

  // dispose uneeded memory
  free(destName);

  /* build the header */
    // a place holder. Will later contain the last byte size
      fwrite(&lastByteFill, sizeof(char), 1, dest);
    // a place holder. Will later contain the compressed file size
      fwrite(&lastByteFill, sizeof(long), 1, dest);
    // contain the frequencies size. We need only 2 bytes. Explenation in the header files' bottom.
      fwrite(&freqMapSize, sizeof(short), 1, dest);

	printf("Constructing the header...");
    // the frequencies
      while(arr -> ch != -1){
        #ifdef DEBUG
          printf("Freq in header: %c : %u\n", arr -> ch, arr -> freq);
        #endif
        fwrite(&(arr -> ch), sizeof(char), 1, dest);
        fwrite(&(arr -> freq), sizeof(unsigned int), 1, dest);
        arr++;
      }
	puts(" Done.");

  #ifdef DEBUG
    puts("compress");
    printInOrder(huffTree);
  #endif

  /* put the content */
  while(fread(&curr, sizeof(char), 1, src) == 1){ // while being able to read a char
    cPath = getCode(curr, huffTree); // get its code ("0100101" as a string)

	#ifdef PROGRESS_BAR_ENABLE
		currPrecantageProg++;
		if(currPrecantageProg == precentageSize){
			currPrecantageProg = 0;
			precentagesNum++;
			updateProgressBar();
		}
	#endif

    #ifdef DEBUG
      printf("\n running on the path of the letter %c: %s\n", curr, cPath);
	  if(curr == '-')
		  puts("last");
    #endif

    while(*cPath != '\0'){ // run on the retured code

      #ifdef DEBUG
        printf("Curr loop char: %c\n", *cPath);
      #endif

      currToInsert <<= 1; // shift left the current value
      currToInsert += (*cPath) - 48; // add at the right 1/0 (accordingly)
      lastByteFill++;

      if(lastByteFill == 8){
        fwrite(&currToInsert, sizeof(char), 1, dest);
        lastByteFill = 0;
      }
      cPath++;
    }
  }

  // write the last byte
  currToInsert <<= 8 - lastByteFill; // move the added bits to be most significant bits
  fwrite(&currToInsert, sizeof(char), 1, dest);

  #ifdef DEBUG
    printf("lbf: %d. it contains %d\n", lastByteFill, currToInsert);
  #endif

  // save the file size
    fileSize = ftell(dest);

  // replace the placeholder at the start with the last byte fill
  rewind(dest);
  fwrite(&lastByteFill, sizeof(char), 1, dest);
  // replace the placeholder at the start with the file size
  fwrite(&fileSize, sizeof(long), 1, dest);

  fclose(dest);
  fclose(src);
}

TNode * buildTreeFromArr(TNode *arr){
  TNode * ret;
  unsigned char s = getMapSize(arr), min = 0, i;

  #ifdef DEBUG
    printf("buildTreeFromArr start. The size: %d\n", s);
    printArr(arr);
  #endif

  if(s == 1)
    return arr;

  // find the min two nodes
  for (i = 0; i < s-1; i++)
    if(arr[i].freq + arr[i+1].freq < arr[min].freq + arr[min+1].freq)
      min = i;

  // create the new middle one
    ret = (TNode *)malloc(sizeof(TNode));
    ret -> freq = arr[min].freq + arr[min+1].freq;

  // copy the min nodes to new memory place, and point the new one on them
    ret -> left = (TNode *)calloc(1, sizeof(TNode));
      copyTN(ret -> left, arr + min);
    ret -> right = (TNode *)calloc(1, sizeof(TNode));
      copyTN(ret -> right, arr + min + 1);

  // copy the new node to the array
    copyTN(arr + min, ret);
    arr[min].ch = '\0';
    free(ret);

  // shift left the array, and make it smaller
  for (i = min+1; i <= s; i++) // <= since I want to copy the end flag node as well
    copyTN(arr + i, arr + i + 1);


  #ifdef DEBUG
    printf("\tSize %d !==! buildTreeFromArr on arr: ",s);
    printArr(arr);
  #endif

  arr = (TNode *)realloc(arr, (s) * sizeof(TNode));
  return buildTreeFromArr(arr);
}

TNode * fileToFreqMap(FILE *f){
  TNode *ret = (TNode *)calloc(256, sizeof(TNode)); // max size.
  unsigned char spaceUsing = 0;
  char curr;

  printf("Reading file and building frequencies map...");

  // While not reaching the end of the file.
  while(fread(&curr, sizeof(char), 1, f) == 1){ // while successfully redd one item

    unsigned char i = 0;
    while(i < spaceUsing && ret[i].ch != curr) // move I to where we should increase
      i++;

    // increase its freq
    ret[i].freq++;
    if(ret[i].ch == '\0'){ // if not setted yet
      ret[i].ch = curr;
      spaceUsing++;
    }

  }

  ret[spaceUsing++].ch = -1;
  ret = (TNode *)realloc(ret, spaceUsing * sizeof(TNode));
  puts(" Done.");

  printf("Quick sorting the map...");
  quickSort(ret, 0, spaceUsing-2);
  puts(" Done.");

  rewind(f);

  #ifdef DEBUG
    printf("The original map: ");
    printArr(ret);
  #endif

  return ret;
}

#ifdef PROGRESS_BAR_ENABLE
	void updateProgressBar(){
		int i;
		printf("\033[2J\033[1;1H");
		toPrint[0] = toPrint[outOf+1] = '|';
		for (i = 1; i <= outOf; i++)
			if(i <= precentagesNum)
				toPrint[i] = '-';
			else
				toPrint[i] = ' ';
		puts(toPrint);
	}
#endif
