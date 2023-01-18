#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "sha2.h"
#define MIN(a,b) ((a < b) ? (a) : (b))
#define MAX(a,b) ((a < b) ? (b) : (a))

typedef unsigned char byte;

unsigned int sha256_h[8] = {
    0x6a09e667UL,
    0xbb67ae85UL,
    0x3c6ef372UL,
    0xa54ff53aUL,
    0x510e527fUL,
    0x9b05688cUL,
    0x1f83d9abUL,
    0x5be0cd19UL
};
 
unsigned int sha256_k[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

unsigned int rightRotateI(unsigned int x, unsigned d)
{
	return (x >> d) | (x << (32 - d));
}

void sha256_initContext(sha256_context *ctx)
{
	ctx->mode = SHA256;
	ctx->msgLen = 0ULL;
	memcpy(ctx->h, sha256_h, sizeof(unsigned int)*8);
	ctx->stateCursor = 0;						 
}

void sha256_f(unsigned int h[8], unsigned char state[SHA256_BLOCK_LEN])
{
	// initialize W
	unsigned int W[16];
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			W[i] <<= 8; // increase magnitude by 1 byte
			W[i] |= state[i * 4 + j];
		}
	}

	// initialize working variables
	unsigned int h2[8];
	memcpy(h2, h, 8* sizeof(unsigned int));


	// go through rounds
	for(int t = 0; t < SHA256_NR; t++)
	{
		int s = t & 0xf; // mod 16

		if(t >= 16)
		{
			// calculate new word
			unsigned int val_s0 = W[(s - 15) & 0xf];
			unsigned int val_s1 = W[(s-2) & 0xf];

			unsigned int s0 = rightRotateI(val_s0, 7) ^ rightRotateI(val_s0, 18) ^ (val_s0 >> 3);
			unsigned int s1 = rightRotateI(val_s1, 17) ^ rightRotateI(val_s1, 19) ^(val_s1 >> 10);

			W[s] = W[s] + s0 + W[(s-7) & 0xf] + s1;
		}
		unsigned int S1 = rightRotateI(h2[4], 6) ^ rightRotateI(h2[4], 11) ^rightRotateI(h2[4], 25);
		unsigned int ch = (h2[4] & h2[5]) ^ (~h2[4] & h2[6]);
		unsigned int tmp1 = h2[7] + S1 + ch + sha256_k[t] + W[s];
		unsigned int S0 = rightRotateI(h2[0], 2) ^ rightRotateI(h2[0], 13) ^ rightRotateI(h2[0], 22);
		unsigned int maj = (h2[0] & h2[1]) ^ (h2[0] &h2[2]) ^ (h2[1] & h2[2]);
		unsigned int tmp2 = S0 + maj;

		for(int i = 7; i > 0; i--)
		{
			h2[i] = h2[i-1];
		}
		h2[4] += tmp1;
		h2[0] = tmp1 + tmp2;
	}

	// update buffer values
	for(int i = 0; i < 8; i++)
	{
		h[i] += h2[i];
		
	}
}

void sha256_update(sha256_context *ctx, unsigned char *in, int n)
{
	int msgCursor = 0;
	while(msgCursor < n)
	{
	    int noBytesInBlock = MIN(SHA256_BLOCK_LEN - ctx->stateCursor, n- msgCursor);
		//copy bytes
		memcpy(ctx->state + ctx->stateCursor, in + msgCursor, noBytesInBlock);
		// advance cursors
		msgCursor += noBytesInBlock;
		ctx->stateCursor += noBytesInBlock;

		if(ctx->stateCursor == SHA256_BLOCK_LEN)
		{
			// reached end of the block

			// call the function
			sha256_f(ctx->h, ctx->state);

			// reset state
			ctx->stateCursor = 0;
		}
	}
	ctx->msgLen += n << 3; // length in bits;
}

void sha256_digest(sha256_context *ctx, unsigned char **out)
{
	// PADDING

	// first bit 1
	ctx->state[ctx->stateCursor++] = 0x80;

	// reset of bits to 0
	memset(ctx->state + ctx->stateCursor, 0, MAX(SHA256_BLOCK_LEN - ctx->stateCursor, 0));

	// output size
	if(ctx->stateCursor >= (SHA256_BLOCK_LEN - sizeof(unsigned long long)))
	{
		// need new block to write complete block

		// call function on complete block
		sha256_f(ctx->h, ctx->state);

		// reset state
		ctx->stateCursor = 0;
		memset(ctx->state, 0, SHA256_BLOCK_LEN);
	}

	// set last 64 bits as length
	unsigned long long size = ctx->msgLen;
	for(int i = SHA256_BLOCK_LEN - 1; size; i--)
	{
		// set LSByte on right most 
		ctx->state[i] = size; // get LSByte of long long
			size >>= 8;      // remove LSByte
	}
	// call function on the last block
	sha256_f(ctx->h, ctx->state);

	// reset state;
	ctx->stateCursor = 0;

	// output is the array ctx->h
	*out = malloc(SHA256_OUT * sizeof(unsigned char));
	if(!(*out))
	{
		// ensure memory was allocated	  
		return;		
	}
	for(int i = 0; i < 8; i++)
	{
		for(int j = 3; j >= 0; j--)
		{
			// get LSByte on the right side
			(*out)[i * 4 + j] = ctx->h[i];
			ctx->h[i] >>= 8; // removes LSByte
		}
	}
}

void printCharArray(unsigned char *arr, int len, int asChar)
{
	char hex[16] = "0123456789ABCDEF";
	printf("{ ");
	for(int i = 0; i < len; i++)
	{
		printf("%c%c ", hex[arr[i] >> 4], hex[arr[i] & 0x0f]);
	}
	printf("}\n");
}
/*
int main(void)
{
	byte *msg = "fr356cqEm7SOqBtvOOOx%MkR&ETUJvuE6AcYNaLSKSLlt6Y4my812pLDk#FEkBMopG5XtoTB6p14kmU6DvsWDT2In5K#wPHW20337021";
	int msg_byte_len = strlen(msg);
	unsigned char *hash1 = NULL;	
	sha256_context ctx1;
	sha256_initContext(&ctx1);
	sha256_update(&ctx1, msg, msg_byte_len);
	sha256_digest(&ctx1, &hash1);

	printf("Q1. hash value = ");
	printCharArray(hash1, SHA256_OUT, 0);
	printf(hash1);
	free(hash1);
	
    const char *filename = "text.txt";
    FILE* input_file = fopen(filename, "r");
    if (!input_file)
        return 0;
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return 0;
    }
    byte *file_contents = malloc(sb.st_size);
    fread(file_contents, sb.st_size, 1, input_file);
    fclose(input_file);    
	int file_byte_len = strlen(file_contents);
	
	sha256_context ctx2;
	unsigned char *hash2 = NULL;	
	sha256_initContext(&ctx2);
	sha256_update(&ctx2, file_contents, file_byte_len);
	sha256_digest(&ctx2, &hash2);
	printf("\nQ2. hash value = ");
	printCharArray(hash2, SHA256_OUT, 0);
	free(hash2);
	free(file_contents);
	return 0;
}
*/

