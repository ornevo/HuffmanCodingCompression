#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "headers.h"

void decompress(char *filename){
  FILE *src = fopen(filename, "rb");
  char *name = getPrevName(filename);
  FILE *dest = fopen(name, "wt"); // open the compressed file and the
  // (continouing the prev comment) dest file
  char lastByteFill, curr = 0, i;
  short freqsSize; // in bytes
  TNode *freqMap;
  TNode *tree, *currNode;
  long srcFileSize;

  /* files opening errors handeling */
  if(!src || !dest){
	  if(!src){
		  fprintf(stderr, "Error opening %s: ", filename);
      remove(name);
    }
	  else
		  fprintf(stderr, "Error writing to %s: ", name);

	  fcloseall();

	  fprintf(stderr, "%s\n", strerror(errno));
	  exit(FILE_ERR_EXIT_CODE);
  } else if(strcmp(getFileExten(filename), "huf")){ // file format check
		fprintf(stderr, "ERROR: The file %s isn't a .huf file, and hence can't be decompressed by this program.\n", filename);
		remove(name);

		exit(FILE_ERR_WRONG_FORMAT);
	}

  free(name);
  lastByteFill = fgetc(src);

  fread(&srcFileSize, sizeof(long), 1, src);
  fread(&freqsSize, sizeof(short), 1, src);
  #ifdef PROGRESS_BAR_ENABLE
	  // determine file size, for progress bar
	  precentageSize = (srcFileSize - freqsSize) / outOf;
  #endif
  freqMap = (TNode *)calloc(freqsSize / 5 + 1, sizeof(TNode)); // since every frequency data takes 5 bytes, theSize/5 gives -->
  // <-- the number of elements

  /* Read the frequencies map from the compressed file */
  printf("Reading frequencies from header...");
  while(curr * 5 < freqsSize){
    fread(&(freqMap[curr].ch), sizeof(char), 1, src); // the char
    fread(&(freqMap[curr].freq), sizeof(unsigned int), 1, src); // the char

	curr++;
  }
  freqMap[curr].ch = -1; // symbolls the end of the array
	puts(" Done.");

  /* tree building */
  printf("Building Huffman tree from frequencies...");
  tree = currNode = buildTreeFromArr(freqMap);
	puts(" Done.");

  /* reading the content */
  while(ftell(src) < srcFileSize){ // while not pointing at the last byte

	#ifdef PROGRESS_BAR_ENABLE
		currPrecantageProg++;
		if(currPrecantageProg == precentageSize){
			currPrecantageProg = 0;
			precentagesNum++;
			printf("\033[2J\033[1;1H");
			printf("Progress: %d%%\n", precentagesNum * 2);
		}
	#endif

    fread(&curr, sizeof(char), 1, src);
    i=8;
    if(ftell(src) == srcFileSize)
      i = lastByteFill;

    while(i-- > 0){ // for each bit in the current
      if(currNode -> ch == '\0'){ // if a crossroad

        if(curr >> 7) // if the next turn is right
          currNode = currNode -> right;
        else
          currNode = currNode -> left;
        curr <<= 1;
      } else {// if reached a leaf

        i++; // since we haven't progressed in the bits, undo the i--
        fputc(currNode -> ch, dest);
        currNode = tree; // return to the base of the tree
      }
    }
  }

  fputc(currNode -> ch, dest); // put the last char.

  fclose(src);
  fclose(dest);
}

char* getPrevName(char *str){
  char prevnamelen = strlen(str)-4;
  char *newstr = (char *)malloc((prevnamelen + 1) * sizeof(char));
  memcpy(newstr, str, (prevnamelen + 1) * sizeof(char));
  newstr[prevnamelen] = '\0';
  return newstr;
}
