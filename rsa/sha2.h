#ifndef _SHA2_
#define _SHA2_

#define SHA256 256
 
#define SHA256_BLOCK_LEN 64

#define SHA256_OUT 256 >> 3
 
#define SHA256_NR 64
 
typedef struct sha256_context
{
	int mode;
	unsigned long long msgLen;

	// buffer for the output
	unsigned int h[8];

	// state values
	int stateCursor;
	unsigned char state[SHA256_BLOCK_LEN];
} sha256_context;

void sha256_initContext(sha256_context *ctx);
void sha256_update(sha256_context *ctx, unsigned char *in, int n);
void sha256_digest(sha256_context *ctx, unsigned char **out);

void sha256_f(unsigned int h[8], unsigned char state[SHA256_BLOCK_LEN]);

#endif
