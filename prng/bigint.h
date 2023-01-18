#ifndef ASDF
#define ASDF
#include <stdint.h>
#define BIGINT_RADIX 0x100000000
#define BIGINT_HALFRADIX 0x80000000

#ifndef NULL
#define NULL ((void *) 0)
#endif

typedef struct
{
	size_t size;
	size_t capacity;
	uint32_t *data;
} bigint_t;

#define BIGINT_DEFAULT_CAPACITY 20

extern bigint_t small_bigint[17];

bigint_t *bigint_alloc_reserve(size_t capacity);
bigint_t *bigint_alloc();
void bigint_free(bigint_t *b);

size_t bigint_get_size(bigint_t *b);

void bigint_load(bigint_t *b, uint8_t *buf, size_t buf_size);
void bigint_save(bigint_t *b, uint8_t *buf);

int bigint_iszero(bigint_t* b);
void bigint_copy(bigint_t *src, bigint_t *dst);
void bigint_fromstring(bigint_t *b, char* str, char base);
void bigint_fromint(bigint_t *b, uint32_t num);
void bigint_print(bigint_t *b, char format);
int bigint_equal(bigint_t *b1, bigint_t *b2);
int bigint_greater(bigint_t *b1, bigint_t *b2);
int bigint_less(bigint_t *b1, bigint_t *b2);
int bigint_geq(bigint_t *b1, bigint_t *b2);
int bigint_leq(bigint_t *b1, bigint_t *b2);
void bigint_iadd32(bigint_t *src, uint32_t b2);
void bigint_add32(bigint_t *result, bigint_t *b1, uint32_t b2);
void bigint_iadd(bigint_t* src, bigint_t* add);
void bigint_add(bigint_t* result, bigint_t* b1, bigint_t* b2);
void bigint_isub(bigint_t* src, bigint_t* add);
void bigint_sub(bigint_t* result, bigint_t* b1, bigint_t* b2);
void bigint_imul(bigint_t* src, bigint_t* add);
void bigint_mul(bigint_t* result, bigint_t* b1, bigint_t* b2);
void bigint_idiv(bigint_t* src, bigint_t* div);
void bigint_idivr(bigint_t* src, bigint_t* div, bigint_t* rem);
void bigint_rem(bigint_t* src, bigint_t *div, bigint_t* rem);
void bigint_imod(bigint_t* src, bigint_t* mod);
void bigint_div(bigint_t* q, bigint_t* rem, bigint_t* b1, bigint_t* b2);
void bigint_modpow(bigint_t *base, bigint_t *exp, bigint_t *mod, bigint_t *result);
//void bigint_gcd(bigint_t *b1, bigint_t *b2, bigint_t *result);
//void bigint_inv(bigint_t *a, bigint_t *m, bigint_t *result);
//int bigint_jacobi(bigint_t *ac, bigint_t *nc);
#endif
