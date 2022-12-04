#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>
typedef unsigned char boolean;
typedef unsigned char byte;
typedef byte *byteptr;
typedef char *string;
typedef unsigned int word32;
typedef unsigned long long word64;

#define SHA1_NR 80 
#define SHA1_BLOCK_LEN 64 // 64 * BYTE = 512 BIT
#define SHA1_OUT 160 >> 3
#define LENGTH_PAD_POS 56

typedef struct sha1_context
{
	word32 h[5];
} sha1_ctx; 

word32 rotateWord(word32 x, unsigned d)
{
	return (x << d) | (x >> (32 - d));
}

void init_context(sha1_ctx *ctx)
{
	ctx->h[0] = 0x67452301;
	ctx->h[1] = 0xEFCDAB89;
	ctx->h[2] = 0x98BADCFE;
	ctx->h[3] = 0x10325476;
	ctx->h[4] = 0xC3D2E1F0;
}

void sha1_block(word32 h[5], byte block[SHA1_BLOCK_LEN])
{ 
	word32 k[4] =
	{
		0x5A827999,
		0x6ED9EBA1,
		0x8F1BBCDC,
		0xCA62C1D6
	};  
	word32 w[SHA1_NR];
	int i, j;
	for(i = 0; i < 16; i++)
	{
		w[i] = 0;
		for(j = 0; j < 4; j++)  // big endian
		{  
		
			w[i] <<= 8; // increase magnitude by 1 byte
			w[i] |= block[i*4+j];
		}
		//printf("W[%d] = %lu  ",i, w[i]); 
	}
	for(i = 16; i < SHA1_NR; i++)
	{
		w[i] = rotateWord(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16],1);
			
	}
	
	word32 a = h[0],
		   b = h[1],
		   c = h[2],
		   d = h[3],
		   e = h[4];
		
	int t;
	for(t = 0; t < SHA1_NR; t++)
	{
		word32 temp, k_val, f_val;    
		
		if(t < 20)
		{
			f_val = ((b & c) | ((~b) & d)); k_val = k[0];
		}
		else if(t < 40)
		{
			f_val = (b ^ c ^ d); k_val = k[1];
		}
		else if(t < 60)
		{
			f_val = ((b & c) | (b & d) | (c & d)); k_val = k[2];
		}
		else if(t < 80)
		{
			f_val = (b ^ c ^ d); k_val = k[3];
		}
		temp = rotateWord(a,5) + e + w[t] + f_val + k_val;

		e = d;
		d = c;
		c = rotateWord(b, 30);
		b = a;
		a = temp;
	}
	h[0] += a;
	h[1] += b;
	h[2] += c;
	h[3] += d;
	h[4] += e;
}

void sha1(sha1_ctx *ctx, byte *data, int byte_len, unsigned char **out)
{ 
	int entire_blocks = byte_len / SHA1_BLOCK_LEN;
	int extra_bytes = byte_len - entire_blocks * SHA1_BLOCK_LEN;	

	init_context(ctx);

	// hash entire blocks
	
	for(int i = 0; i < entire_blocks; i++)
	{
		sha1_block(ctx->h, data + i*SHA1_BLOCK_LEN);
	}	
	
	byte pad_block[SHA1_BLOCK_LEN];		

	int last_block_pos = entire_blocks*SHA1_BLOCK_LEN;
	for(int i = 0; i < SHA1_BLOCK_LEN; i++)
	{
		if(i < extra_bytes)
			pad_block[i] = data[last_block_pos+i];
		else
			pad_block[i] = 0;
	}

	int pad_block_pos = extra_bytes;
	pad_block[pad_block_pos++] = 0x80;

	if(pad_block_pos >= LENGTH_PAD_POS)
	{		
		sha1_block(ctx->h, pad_block); 
		memset(pad_block, 0, sizeof(byte)*SHA1_BLOCK_LEN);			
	}			

	word64 bit_length = (word64)byte_len*8;

	for(int i = LENGTH_PAD_POS +7; i >= LENGTH_PAD_POS; i--)
	{
		// set LSByte on rightmost slot
		pad_block[i] = bit_length; // gets LSByte of long long
		bit_length >>= 8;          // remove LSBytex
	}
	// call function on last block
	sha1_block(ctx->h, pad_block);

    *out = malloc(SHA1_OUT * sizeof(byte));
	if(!(*out))
	{
		// ensure memory was allocated
		return;
	}
	
	for(int i = 0; i < 5; i++)
	{
		word32 temp = ctx->h[i];
		for(int j = 3; j >= 0; j--)
		{
			// get LSByte on right side
			(*out)[i*4 + j] = temp;
			temp >>= 8;
		}
	}	
}

void printCharArray(unsigned char *arr, int len, boolean asChar)
{
	char hex[16] = "0123456789ABCDEF";
	printf("{ ");
	for(int i = 0; i < len; i++)
	{
		printf("%c%c ", hex[arr[i] >> 4], hex[arr[i] & 0x0f]);
	}
	printf("}\n");
}

int main(void)
{
	sha1_ctx ctx1;
	byte *msg = "fr356cqEm7SOqBtvOOOx%MkR&ETUJvuE6AcYNaLSKSLlt6Y4my812pLDk#FEkBMopG5XtoTB6p14kmU6DvsWDT2In5K#wPHW20337021";
	int msg_byte_len = strlen(msg);
	unsigned char *hash1 = NULL;
	sha1(&ctx1, msg, msg_byte_len, &hash1);
	printf("Q1. hash value = ");
	printCharArray(hash1, SHA1_OUT, 0);
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
	
	sha1_ctx ctx2;
	unsigned char *hash2 = NULL;	
	sha1(&ctx2, file_contents, file_byte_len, &hash2);
	printf("\nQ2. hash value = ");
	printCharArray(hash2, SHA1_OUT, 0);
	free(hash2);
	free(file_contents);
	return 0;
}
