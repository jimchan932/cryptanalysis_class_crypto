#include "bigint.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
uint32_t small_bigint_data[17][1] = {
    {0x0}, {0x1}, {0x2}, {0x3}, {0x4}, {0x5}, {0x6}, {0x7},
    {0x8}, {0x9}, {0xa}, {0xb}, {0xc}, {0xd}, {0xe}, {0xf},
    {0x10}
};

bigint_t small_bigint[17] = {
    {1, 1, small_bigint_data[0]},
    {1, 1, small_bigint_data[1]},
    {1, 1, small_bigint_data[2]},
    {1, 1, small_bigint_data[3]},
    {1, 1, small_bigint_data[4]},
    {1, 1, small_bigint_data[5]},
    {1, 1, small_bigint_data[6]},
    {1, 1, small_bigint_data[7]},
    {1, 1, small_bigint_data[8]},
    {1, 1, small_bigint_data[9]},
    {1, 1, small_bigint_data[10]},
    {1, 1, small_bigint_data[11]},
    {1, 1, small_bigint_data[12]},
    {1, 1, small_bigint_data[13]},
    {1, 1, small_bigint_data[14]},
    {1, 1, small_bigint_data[15]},
    {1, 1, small_bigint_data[16]}
};


size_t max(size_t a, size_t b)
{
	return (a < b) ? a : b;
}


bigint_t *bigint_alloc()
{
	return bigint_alloc_reserve(BIGINT_DEFAULT_CAPACITY);
}
	
size_t bigint_get_size(bigint_t *b)
{
	return b->size*4;
}
	
void bigint_reserve(bigint_t *b, size_t size)
{
	if(b->capacity < size)
	{
		b->capacity = size;
		b->data = realloc(b->data, size*sizeof(uint32_t));
	}
}

bigint_t *bigint_alloc_reserve(size_t capacity)
{
	bigint_t *b = malloc(sizeof(bigint_t) + capacity*sizeof(uint32_t));
	b->size = 1;
	b->capacity = capacity;
	b->data = calloc(capacity, sizeof(uint32_t));
	return b;
}
// Load a bigint from an unsigned integer.
void bigint_fromint(bigint_t *b, uint32_t num)
{
    b->size = 1;
    bigint_reserve(b, b->size);
    b->data[0] = num;
}


void bigint_free(bigint_t *b)
{
	free(b->data);
	free(b);
}

void bigint_load(bigint_t *b, uint8_t *buf, size_t buf_size)
{
	b->size = buf_size / 4;
	bigint_reserve(b, b->size);
	memcpy(b->data, buf, b->size*sizeof(uint32_t));
}

void bigint_save(bigint_t *b, uint8_t *buf)
{
	memcpy(buf, b->data, b->size*sizeof(uint32_t));
}

int bigint_iszero(bigint_t *b)
{
	return !b->size || b->size && !b->data[0];
}
void bigint_copy(bigint_t *src, bigint_t *dst)
{
	dst->size = src->size;
	bigint_reserve(dst, src->capacity);
	memcpy(dst->data, src->data, dst->size*sizeof(uint32_t));
}

void bigint_from_string(bigint_t *b, char *str, char base)
{
	size_t len = strlen(str);
	size_t base_size = base == 'd' ? 10 : 16;
	for(size_t i = 0; i < len; i++)
	{
		if(i)
		{
			bigint_imul(b, &small_bigint[base_size]);			
		}
		char c = str[i];
		if(c >= 'a')
			c = c - 'a' + 10;
		else
			c = c - '0';
		bigint_iadd(b, &small_bigint[c]);
	}
}

int bigint_equal(bigint_t *b1, bigint_t *b2)
{
	if(bigint_iszero(b1) && bigint_iszero(b2))
		return 1;
	else if(bigint_iszero(b1))
		return 0;
	else if(bigint_iszero(b2))
		return 0;
	else if(b1->size != b2->size)
		return 0;
	for(size_t i = b1->size; i--;)
	{
		if(b1->data[i] != b2->data[i])
			return 0;
	}
	return 1;
}

int bigint_greater(bigint_t *b1, bigint_t *b2)
{
	if(bigint_iszero(b1) && bigint_iszero(b2))
		return 0;
	else if(bigint_iszero(b1))
		return 0;
	else if(bigint_iszero(b2))
		return 1;
	else if(b1->size != b2->size)
		return b1->size > b2->size;
    for (size_t i = b1->size; i--;)
    {
        if (b1->data[i]!=b2->data[i])
            return b1->data[i] > b2->data[i];
    }
    return 0;
}

int bigint_less(bigint_t *b1, bigint_t *b2)
{
	if(bigint_iszero(b1) && bigint_iszero(b2))
		return 0;
	else if(bigint_iszero(b1))
		return 1;	        
    else if (bigint_iszero(b2))
        return 0;
    else if (b1->size!=b2->size)
        return b1->size < b2->size;
    for (size_t i = b1->size; i--;)
    {
        if (b1->data[i]!=b2->data[i])
            return b1->data[i] < b2->data[i];
    }
    return 0;
}

int bigint_geq(bigint_t *b1, bigint_t *b2)
{
	return !bigint_less(b1,b2);
}

int bigint_leq(bigint_t *b1, bigint_t *b2)
{ return !bigint_greater(b1, b2); }


void bigint_iadd32(bigint_t *src, uint32_t b2)
{
	bigint_t *temp = bigint_alloc();
	bigint_add32(temp, src, b2);
	bigint_copy(temp, src);
	bigint_free(temp);
}

void bigint_add32(bigint_t *result, bigint_t *b1, uint32_t b2)
{	
	size_t n = b1->size;
	bigint_reserve(result, n+1);
	uint32_t carry = 0;
    for(size_t i = 0; i < n; i++)
	{
		uint32_t sum = carry;
		if(i < b1->size)
			sum += b1->data[i];
		if(i < 1)
			sum += b2;
		// already taken mod 2^32 by unsigned wraparound
		result->data[i] = sum;
		// Result must have wrapped by 2^32 so carry bit is 1
		if(i < b1->size)
			carry = sum < b1->data[i];
		else
			carry = sum < b2;			
	}
	if(carry == 1)
	{
		result-> size = n+1;
		result->data[n] = 1;
	}
	else
		result->size = n;
}

void bigint_iadd(bigint_t *src, bigint_t *add)
{
	bigint_t *temp = bigint_alloc();
	bigint_add(temp, src, add);
	bigint_copy(temp, src);
	bigint_free(temp);
}

// Add two bigints by the add with carry method
// result = b1 + b2
void bigint_add(bigint_t *result, bigint_t *b1, bigint_t *b2)
{
	size_t n = max(b1->size, b2->size);
	bigint_reserve(result, n+1);
	uint32_t carry = 0;
	for(size_t i = 0; i < n; i++)
	{
		uint32_t sum = carry;
		if(i < b1->size)
			sum += b1->data[i];
		if(i < b2->size)
			sum += b2->data[i];

		// already taken mod 2^32 by unsigned wrap around
		result->data[i] = sum;
		// result jmust have wrapped 2^32 so carry bit is 1
		if(i < b1->size)
			carry = sum < b1->data[i];
		else
			carry = sum < b2->data[i];
	}
	if(carry == 1)
	{
		result->size = n+1;
		result->data[n] = 1;
	}
	else
	{
		result->size = n;
	}
}

void bigint_sub(bigint_t *dst, bigint_t *b1, bigint_t *b2)
{
    size_t length = 0;
    uint32_t carry = 0, diff, temp;
    bigint_reserve(dst, b1->size);
    for (size_t i = 0; i < b1->size; i++)
    {
        temp = carry;
        if (i < b2->size)
            temp += b2->data[i]; // Auto wrapped mod RADIX
        diff = b1->data[i]-temp;
        carry = temp > b1->data[i];
        dst->data[i] = diff;
        if (dst->data[i])
            length = i;
    }
    dst->size = length+1;
}

void bigint_imul(bigint_t *src, bigint_t *mult)
{
	bigint_t *temp = bigint_alloc();
	bigint_mul(temp, src, mult);
	bigint_copy(temp, src);
	bigint_free(temp);
}

void bigint_mul(bigint_t *dst, bigint_t *b1, bigint_t *b2)
{
	size_t comp_size = b1->size + b2->size;
	bigint_reserve(dst, comp_size);
	memset(dst->data, 0, comp_size*sizeof(uint32_t));
	for(int i = 0; i < b1->size; i++)
	{
		for(int j = 0; j < b2->size; j++)
		{
			// 这不应该有Overflow
			uint64_t prod = b1->data[i] * (uint64_t)b2->data[j] + (uint64_t)dst->data[i+j];
			uint32_t carry = (uint32_t)(prod/BIGINT_RADIX);
			// Add carry to the next utin32_t over, but this may cause overflow
			// further overflow
			int k = 1;
			while(carry)
			{
				uint32_t temp = dst->data[i+j+k] + carry;
				carry = temp < dst->data[i+j+k] ? 1 : 0;
				dst->data[i+j+k] = temp;
				k++;
			}
			prod = (dst->data[i+j] + b1->data[i]*(uint64_t)b2->data[j]) % BIGINT_RADIX;
			dst->data[i+j] = (uint32_t) prod; // add
		}		
	}
	dst->size = comp_size;
	if(dst->size && !dst->data[dst->size-1])
		dst->size--;
}
uint32_t get_shifted_block(bigint_t *num, size_t x, uint32_t y)
{
    uint32_t part1 = !x || !y ? 0 : num->data[x-1] >> (32-y);
    uint32_t part2 = x==num->size ? 0 : num->data[x] << y;
    return part1|part2;
}

void strip_leading_zeros(bigint_t *num)
{
    while (num->size && !num->data[num->size-1])
        num->size--;
}
void bigint_div(bigint_t *q, bigint_t *rem, bigint_t *b1, bigint_t *b2)
{
    if (bigint_less(b1, b2))
    {
        // Trivial case: b1/b2 = 0 if b1<b2.
        bigint_copy(&small_bigint[0], q);
        bigint_copy(b1, rem);
        return;
    }
    if (bigint_iszero(b1) || bigint_iszero(b2))
    {
        // let a/0 == 0 and a%0 == a to preserve these two properties:
		// 1] a%0 == a
		// 2] (a/b)*b + (a%b) == a
        bigint_copy(&small_bigint[0], q);
        bigint_copy(&small_bigint[0], rem);
        return;
    }
    // Thanks to Matt McCutchen <matt@mattmccutchen.net> for the algorithm.
    size_t ref_size = b1->size;
    bigint_copy(b1, rem);
    bigint_reserve(rem, rem->size+1);
    rem->size++;
    rem->data[ref_size] = 0;
    uint32_t *sub_buf = malloc(rem->size*sizeof(uint32_t));
    q->size = ref_size - b2->size+1;
    bigint_reserve(q, q->size);
    memset(q->data, 0, q->size*sizeof(uint32_t));
    // for each left-shift of b2 in blocks...
    size_t i = q->size;
    while (i)
    {
        i--;
        // for each left-shift of b2 in bits...
        q->data[i] = 0;
        uint32_t i2 = 32;
        while (i2>0)
        {
            i2--;
            size_t j, k;
            int borrow_in, borrow_out;
            for (j = 0, k = i, borrow_in = 0; j<=b2->size; j++, k++)
            {
                uint32_t temp = rem->data[k]-get_shifted_block(b2, j, i2);
                borrow_out = temp > rem->data[k];
                if (borrow_in)
                {
                    borrow_out |= !temp;
                    temp--;
                }
                sub_buf[k] = temp;
                borrow_in = borrow_out;
            }            
            for (; k<ref_size && borrow_in; k++)
            {
                borrow_in = !rem->data[k];
                sub_buf[k] = rem->data[k]-1;
            }
            if (!borrow_in)
            {
                q->data[i] |= 1 << i2;
                while (k>i)
                {
                    k--;
                    rem->data[k] = sub_buf[k];
                }
            }
        }
    }
    strip_leading_zeros(q);
    strip_leading_zeros(rem);
    free(sub_buf);
}
// src /= div
void bigint_idiv(bigint_t *src, bigint_t *div)
{
	bigint_t *q = bigint_alloc();
	bigint_t *r = bigint_alloc();
	bigint_div(q, r, src, div);
	bigint_copy(q, src);
	bigint_free(q);
}

// in place divide of source, also producing a remainder
// src = src / div, rem = src % div
void bigint_idvir(bigint_t *src, bigint_t *div, bigint_t *rem)
{
	bigint_t *q = bigint_alloc();
	bigint_t *r = bigint_alloc();
	bigint_div(q, r, src, div);
	bigint_copy(q, src);
	bigint_copy(r, rem);
	bigint_free(q);
	bigint_free(r);
}

// src %= mod
void bigint_imod(bigint_t *src, bigint_t *mod)
{
	bigint_t *q = bigint_alloc();
	bigint_t *r = bigint_alloc();
	bigint_div(q, r, src, mod);
	bigint_copy(r, src);
	bigint_free(q);
	bigint_free(r);
}


void bigint_modpow(bigint_t *base, bigint_t *exp, bigint_t *mod,
				   bigint_t *result)
{
	// 我们要计算  base ^ exp mod (mod)
	bigint_t *a = bigint_alloc();
	bigint_t *b = bigint_alloc();
	bigint_t *c = bigint_alloc();
	bigint_t *discard = bigint_alloc();
	bigint_t *remainder = bigint_alloc();
	// a^b mod c
	bigint_copy(base, a);     // 低
	bigint_copy(exp, b);     // 指数
	bigint_copy(mod, c);    // 模数
	bigint_fromint(result, 1); 
	
	while(bigint_greater(b, &small_bigint[0])) // exponent greater than 0
	{
		if((b->data[0] % 2) == 0) // if exponent 
		{
			// result := (result * base) mod modulus
			bigint_imul(result, a);
			bigint_imod(result, c);
		}
		bigint_idiv(b, &small_bigint[2]);  // 左移1位, 等同于除于2

		// base := (base * base) mod modulus
		bigint_copy(a, discard); 
		bigint_imul(a, discard);
		bigint_imod(a, c);
	}

	//清除变量 free up heap
	bigint_free(a);
	bigint_free(b);
	bigint_free(c);
	bigint_free(discard);
	bigint_free(remainder);
}

 int mod2(bigint_t *n)
{
	return n->data[0] % 2;
}
int main()
{
	char *p_str = "38276975027249116051285900801331379141255433763584992333552216611485936638623";
    char *q_str = "170141183460469231731688582683058187039";
	char *g_str = "15593026659801183422297598290145472779507237111936928372786422955200978648323";
	char *student_number_string = "20337021";
	bigint_t *g, *p, *q;
	bigint_t *seed;
	
	int z[512];
	g = bigint_alloc();
	p = bigint_alloc();
	q = bigint_alloc();
	seed = bigint_alloc();
	bigint_from_string(g, g_str, 'd');
	bigint_from_string(q, q_str, 'd');
	bigint_from_string(p, p_str, 'd');
	bigint_from_string(seed, student_number_string, 'd'); // initialize seed
	bigint_t *s, *new_s;
	s = bigint_alloc();
	bigint_copy(seed, s);
	int zero = 0; int one = 0;

	double old = clock();
	for(int i = 0; i < 512
			; i++)
	{
		new_s = bigint_alloc();
		bigint_modpow(g, s, p, new_s);
		bigint_imod(new_s, q);
		z[i] = mod2(new_s); // 要求模， 只需要比较最低位
		if(z[i] == 0) zero++;
		else one++;
		bigint_copy(new_s, s);
		bigint_free(new_s);
	}
	clock_t new = clock();
	printf("Random number: \n");
	for(int i =0; i< 512; i++)
	{
		printf("%d", z[i]);
	}
	
	printf("Num of zeroes: %d", zero);
	printf("Num of ones: %d", one);
	double runningtime = (new - old) / CLOCKS_PER_SEC;
		printf("\nRun time: %lf", runningtime);
	return 0;
}
