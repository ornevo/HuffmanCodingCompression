/* TODO:
 * free all memory
 * handle file errors
 */

typedef struct TNode{
  char ch;
  unsigned int freq;
  struct TNode *right, *left;
} TNode;

static const int NOT_VALID_OPTION_EXIT_CODE = 1;
static const int FILE_ERR_EXIT_CODE = 2;
static const int FILE_ERR_WRONG_FORMAT = 3;
static const int EMPTY_FILE_ERR = 4;
static const int WRONG_NUMBER_OF_PARAMETERS = 5;

#define PROGRESS_BAR_ENABLE
#ifdef PROGRESS_BAR_ENABLE
	static int precentageSize, precentagesNum, currPrecantageProg;
	static const int outOf = 50;
	static char *toPrint;
#endif

/**** COMPRESS.C ****/
/*
 * Returnes a Huffman Tree according to the map passed as argument.
 * The size is the number of items in the map, not including the item that
 * indicates the end (with the char of -1).
*/
TNode * buildTreeFromArr(TNode *arr);

/*
 * Returnes an array of Freq structuress.
 * The returned array is sorted low-to-high by the freqs.
 * The last freq in the array has a char of -1;
 * The file f should be already open
*/
 TNode * fileToFreqMap(FILE *f);

/* TODO: ADD DESCRIPTION
 * NOTE: Opens the given file internally
 */
void compress(char *fileName);

/**** DECOMPRESS.C ****/
void decompress(char *filename);
char* getPrevName(char *str);

/**** HELPERS.C ****/

  /* Quick sorts the freqs array */
    void quickSort(TNode *arr, unsigned char left, unsigned char right);

  /* Allocates memory and creates a new TNode */
    TNode * newTNode(char info, unsigned char freq);

  /* Printing functions */
    void printInOrder(TNode *t);

    char * getInOrderStr(TNode *t);

    void printArr(TNode *arr);

  /* Copy an node. copies b to a. */
    void copyTN(TNode *a, TNode *b);

  /* for compress.c */
    /* Takes the originall name, and returns the proper name of the new file */
      char * getDestName(char *src);

    /* Retures the number of items in the map, not including the node that symbolls the end */
      unsigned char getMapSize(TNode *t);

    /* returns the code of the char as a string */
      char * getCode(char c, TNode *root);

    /* cloning frequency maps */
      TNode * cloneMap(TNode *f);

    /* giving the files extension */
      char* getFileExten(char *);

/* !--  HEADER STRUCTURE  --! */
/*
___________________
|  LAST BYTE FILL  | // one byte
|------------------|
|  RES FILE SIZE   | // eight bytes.
|------------------|
| FREQUENCIES SIZE | // two bytes. We only need two bytes since we have 255 chars, and every char has a frequency that takes 4 bytes.
|------------------| // Hence, the max size of that part is (1+4)*255 = 1,275, which can be saved in 2 bytes.
|  FREQUENCIES MAP | // In the format of | char ch | unsigned int freq | char ch | unsigned int freq ....
|__________________|

*/

void updateProgressBar();
