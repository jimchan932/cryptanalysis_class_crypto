#ifndef _SHA2_
#define _SHA2_

#define SHA224 224
#define SHA256 256
#define SHA384 384
#define SHA512 512
 
#define SHA224256_BLOCK_LEN 64

 
#define SHA224_OUT 224 >> 3
#define SHA256_OUT 256 >> 3
 
#define SHA224256_NR 64
 
typedef struct sha224256_context
{
	int mode;
	unsigned long long msgLen;

	// buffer for the output
	unsigned int h[8];

	// state values
	int stateCursor;
	unsigned char state[SHA224256_BLOCK_LEN];
} sha224256_context;

typedef sha224256_context sha256_context;

void sha256_initContext(sha256_context *ctx);
void sha256_update(sha256_context *ctx, unsigned char *in, int n);
void sha256_digest(sha256_context *ctx, unsigned char **out);

void sha224256_f(unsigned int h[8], unsigned char state[SHA224256_BLOCK_LEN]);

#endif
