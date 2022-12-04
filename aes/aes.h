#include  "cmathematics.h"

#ifndef AES_H
#define AES_H

#define BLOCK_LEN 16
#define BLOCK_SIDE 4

#define AES_128 128
#define AES_192 192
#define AES_256 256

/*
  REFERENCE TABLES
 */


// substitution box
extern unsigned char s_box[256];

// constant matrix for mix columns
extern unsigned char mixColMat[BLOCK_SIDE][BLOCK_SIDE];


/*
UTILITY METHODS
*/
unsigned char galoisMul(unsigned char g1, unsigned char g2);

/*
AES ENCRYPTION LAYERS
 */

void addRoundKey(unsigned char state[BLOCK_SIDE][BLOCK_SIDE], unsigned char subkey[BLOCK_SIDE][BLOCK_SIDE]);
void byteSub(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);
void shiftRows(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);
void mixCols(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);

void aes_encrypt_block(unsigned char *in_text, int n,
					   unsigned char subkeys[][BLOCK_SIDE][BLOCK_SIDE], int nr,
					   unsigned char *init_vector,
					   unsigned char out_start[BLOCK_LEN], unsigned char prevOut[BLOCK_LEN],unsigned char out[BLOCK_LEN]);

int aes_encrypt(unsigned char *in_text, int n,
				unsigned char *in_key, int keylen,
				unsigned char *init_vector,
				unsigned char **out);

/* 
   KEY SCHEDULING
 */
void generateKeySchedule(unsigned char *in_key, int keylen, unsigned char subkeys[][BLOCK_SIDE][BLOCK_SIDE]);
void generateKeySchedule128(unsigned char *in_key, unsigned char subkeys[11][BLOCK_SIDE][BLOCK_SIDE]);

void printCharArr(unsigned char *arr, int len, bool asChar);
#endif
