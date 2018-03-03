// #define DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

/* !-- GENERAL --! */

void printInOrder(TNode *t){
  if(!t)
    return;
  printInOrder(t -> left);
  if(t -> ch <= 32)
    printf("\\%d ", (int)(t -> ch));
  else
    printf("%c ", t -> ch);
  printInOrder(t -> right);
}

char * getInOrderStr(TNode *t){
  char * ret, * tmp;
  if(!t)
    return (char *)calloc(1, sizeof(char)); // return an empty string

  ret = getInOrderStr(t -> left);

  ret = (char *)realloc(ret, (strlen(ret) + 2) * sizeof(char));
  ret[strlen(ret) + 1] = '\0';
  ret[strlen(ret)] = t -> ch;

  tmp = getInOrderStr(t -> right);
  ret = (char *)realloc(ret, (strlen(ret) + strlen(tmp) + 1) * sizeof(char));
  memcpy(ret + strlen(ret), tmp, (strlen(tmp) + 1) * sizeof(char));
  free(tmp);

  return ret;
}

void printArr(TNode *arr){
  while(arr -> ch != -1){
    if((arr -> ch) <= 32)
      printf(" \\%d:", (int)(arr -> ch));
    else
      printf(" %c:", arr -> ch);
    printf("%u ", arr -> freq);
    arr++;
  }

  printf(" %d:", (int)(arr -> ch));
  printf("%u ", arr -> freq);

  puts("");
}

TNode * newTNode(char info, unsigned char freq){
  TNode *ret = (TNode *)malloc(sizeof(TNode));
  ret -> ch = info;
  ret -> right = ret -> left = NULL;
  ret -> freq = freq;
  return ret;
}

void copyTN(TNode *a,TNode *b){
  a -> ch = b -> ch;
  a -> left = b -> left;
  a -> right = b -> right;
  a -> freq = b -> freq;
}

/* !-- COMPRESS.C --! */

void quickSort(TNode *arr, unsigned char left, unsigned char right) {
      unsigned char i = left, j = right;
      TNode tmp, piv = arr[(left + right) / 2];

      while (i <= j){
            while (arr[i].freq > piv.freq)
                  i++;
            while (arr[j].freq < piv.freq)
                  j--;

            if (i <= j) {
                  tmp = arr[i];
                  arr[i] = arr[j];
                  arr[j] = tmp;
                  i++;
                  j--;
            }
      }

      if (left < j)
            quickSort(arr, left, j);
      if (i < right)
            quickSort(arr, i, right);
}

char * getDestName(char *src){
  /*
  char *ret = strtok(src, '.');
  char *toAdd;
  // run on each word in the name seperated with '.'
    ret = (char *)realloc(ret, strlen(ret) + strlen(toAdd) + 2); // the + 2 is for the \0 char and the '.' between them.
    while((toAdd = strtok(NULL, '.')) && toAdd != ".txt"){
    strcat(ret, '.');
    strcat(ret, toAdd);
  }
  ret = (char *)realloc(ret, strlen(ret) + 5); // the + 5 is for the \0 char and the '.bin' at the end
  strcat(ret, ".bin");
*/
  char * ret = (char *)malloc(strlen(src) + 5); // the + 5 is for the \0 char and the '.bin' at the end
  char * toAppend = ".huf";
  strcpy(ret, src);
  strcat(ret, toAppend);
  return ret;
}

unsigned char getMapSize(TNode *t){
  int size = 0;

  #ifdef DEBUG
    puts("getMapSize");
  #endif

  while(t -> ch != -1){
    size++;
    t++;
  }

  #ifdef DEBUG
    printf("Finished GMS, and the size is %d\n", size);
    printArr(t);
  #endif

  return size;
}

char * getCode(char c, TNode *root){
  char * ret, toAdd = '3';

  #ifdef DEBUG
    printf("getCode with char %c and a tree of ", 'a');
    printInOrder(root);
  #endif

  if(root -> ch != '\0'){ // if a leaf
    if(root -> ch != c)
      return NULL; // return NULL if the wrong leaf

    // return an empty string, to indicate the the leaf is here
    ret = (char *)malloc(sizeof(char));
    ret[0] = '\0';
    return ret;
  }

  ret = getCode(c, root -> left); // check left first, since there is a better chance its there
  if(ret) // if found on the left
    toAdd = '0';
  else if(ret = getCode(c, root -> right)) // check right
    toAdd = '1';

  if(toAdd != '3'){ // if found on the left
    ret = (char *)realloc(ret, (strlen(ret) + 2) * sizeof(char)); // plus 2 to add the extra 0 and the \0
    memmove(ret + 1, ret, (strlen(ret) + 1) * sizeof(char)); // + 1 for the \0
    ret[0] = toAdd;
    return ret;
  }
  return NULL;
}

TNode * cloneMap(TNode *map){
  TNode *ret = (TNode *)calloc((getMapSize(map)+1), sizeof(TNode)), *curr = ret;
  while (map -> ch != -1)
    copyTN(curr++, map++);
  curr -> ch = -1;
  return ret;
}

char* getFileExten(char *f){ return &(f[strlen(f)-3]); }
