#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define GET_MSB(x) (x >> 8) & 1  // little endian
#define ARRAY_SIZE 2
#define N 8
#define TOTAL_BITS 16
typedef int8_t byte; 

byte primitivePolynomial[ARRAY_SIZE];
void initPrimitivePolynomial()
{
	// x^131 + x^13 + x^2 + x + 1
	memset(primitivePolynomial, 0, sizeof(byte)*ARRAY_SIZE);
	primitivePolynomial[1] = primitivePolynomial[1] | (1 << 0); 	
	primitivePolynomial[0] = primitivePolynomial[0] | (1 << 4);
	primitivePolynomial[0] = primitivePolynomial[0] | (1 << 3);
	primitivePolynomial[0] = primitivePolynomial[0] | (1 << 1);
	primitivePolynomial[0] = primitivePolynomial[0] | (1 << 0);		
}

bool getBit(byte polynomial[ARRAY_SIZE], int n)
{
	int index = n / 8;
	return (polynomial[index] >> (n % 8)) & 1;
}

void setBit(byte polynomial[ARRAY_SIZE], int n)
{
	int index = n / 8;
	polynomial[index] = polynomial[index] | (1 << (n % 8));
}
void toggleBit(byte polynomial[ARRAY_SIZE], int n)
{
	int index = n / 8;
	polynomial[index] = polynomial[index] ^ (1 << (n % 8));
}

bool equalZero(byte polynomial[ARRAY_SIZE])
{
	for(int i = 0; i < ARRAY_SIZE; i++)
	{
		if(polynomial[i] != 0)
			return false;
	}
	return true;	
}

void xor(byte result[ARRAY_SIZE], byte a[ARRAY_SIZE], byte b[ARRAY_SIZE])
{
	for(int i = 0; i < ARRAY_SIZE; i++)
	{
		result[i] = a[i] ^ b[i];
	}
}
void addPolynomial(byte result[ARRAY_SIZE], byte a[ARRAY_SIZE], byte b[ARRAY_SIZE])
{
	xor(result, a, b);
}
void copyPolynomial(byte result[ARRAY_SIZE], byte polynomial[ARRAY_SIZE])
{
	for(int i = 0; i < ARRAY_SIZE; i++)
	{
		result[i] = polynomial[i];
	}
}
void leftShiftPolynomial(byte result[ARRAY_SIZE], byte polynomial[ARRAY_SIZE])
{
	bool shiftCarry = 0;
	for(int i = 0; i < ARRAY_SIZE; i++)
	{
		bool temp = GET_MSB(polynomial[i]);
		result[i] = polynomial[i] << 1; 
		result[i] = result[i] | (shiftCarry << 0);
		shiftCarry = temp;				
	}
	
	if(getBit(result, N) == 1) 
		addPolynomial(result, result, primitivePolynomial);
}

void leftShiftPolynomialByAmount(byte result[ARRAY_SIZE],
								 byte polynomial[ARRAY_SIZE],
								 int shiftAmount)
{
	
	if(shiftAmount == 0)
	{
		copyPolynomial(result, polynomial);
		return;
	}
	byte tempPolynomial[ARRAY_SIZE];
	copyPolynomial(tempPolynomial, polynomial);
	for(int i = 0; i < shiftAmount; i++)
	{
		bool shiftCarry = 0;
		for(int j = 0; j < ARRAY_SIZE; j++)
		{
			bool temp = GET_MSB(tempPolynomial[j]);
			result[j] = tempPolynomial[j] << 1; 
			result[j] = result[j] | (shiftCarry << 0);
			shiftCarry = temp;
		}	
		copyPolynomial(tempPolynomial, result);
	}

}
void printPolynomial(byte polynomial[ARRAY_SIZE])
{
	printf("[");
	for(int i = TOTAL_BITS - 1; i >= 0; i--){
		if(getBit(polynomial, i) == 1)
		{
			printf("%d,", i); 
		}
	}
	printf("]");
}
void multiplyPolynomial(byte result[ARRAY_SIZE], byte a[ARRAY_SIZE], byte b[ARRAY_SIZE])
{
    byte copiedA[ARRAY_SIZE], copiedB[ARRAY_SIZE];	
	memset(result, 0, sizeof(byte)*ARRAY_SIZE);
	copyPolynomial(copiedA, a);
	copyPolynomial(copiedB, b);
	for(int i = 0; i < N; i++)
	{
		if(getBit(copiedA, i) == 1)
			addPolynomial(result, result, copiedB);
		leftShiftPolynomial(copiedB, copiedB);
	}
}

int getDegree(byte polynomial[ARRAY_SIZE])
{
	int degree;
	if(equalZero(polynomial)) return -1;
	for(int i = TOTAL_BITS - 1; i >= 0; i--)
	{
		if(getBit(polynomial, i) == 1)
		{
			degree = i;
			break;
		}
	}
	return degree;
}

// a = bq + r, deg(a) < deg(b)
void euclideanAlgorithm(byte quotient[ARRAY_SIZE], byte remainder[ARRAY_SIZE], byte a[ARRAY_SIZE], byte b[ARRAY_SIZE])
{
	byte tempProduct[ARRAY_SIZE];
    memset(quotient, 0, sizeof(byte) * ARRAY_SIZE);    
	copyPolynomial(remainder, a);
   	
	int degreeB = getDegree(b);
	int degreeOffset;	
	while((degreeOffset = getDegree(remainder) - degreeB) >= 0)
	{
		int index = degreeOffset / 8;
		// quotient[index] = quotient[index] | (1 << (degreeOffset% 8)); // IMPORTANT !!
		setBit(quotient, degreeOffset);
		leftShiftPolynomialByAmount(tempProduct, b, degreeOffset);	
		addPolynomial(remainder, remainder, tempProduct);
	}	
}

void extendedEuclidenaAlgorithm(byte s[ARRAY_SIZE], byte t[ARRAY_SIZE], byte a[ARRAY_SIZE], byte b[ARRAY_SIZE])
{
	byte a_0[ARRAY_SIZE], b_0[ARRAY_SIZE], t_0[ARRAY_SIZE], s_0[ARRAY_SIZE],
		q[ARRAY_SIZE], r[ARRAY_SIZE], temp[ARRAY_SIZE], tempProduct[ARRAY_SIZE];

	copyPolynomial(a_0, a);
	copyPolynomial(b_0, b);
	memset(t_0, 0, sizeof(byte) *ARRAY_SIZE);
	memset(s_0, 0, sizeof(byte) *ARRAY_SIZE);
	memset(t, 0, sizeof(byte) *ARRAY_SIZE);
	memset(s, 0, sizeof(byte) *ARRAY_SIZE);	
	t[0] = s_0[0] = 1;
	
	while(!equalZero(r))
	{
		euclideanAlgorithm(q, r, a_0, b_0);
		multiplyPolynomial(tempProduct, q, t); // qt
		addPolynomial(temp, t_0, tempProduct); // temp = t_0 - qt
		copyPolynomial(t_0, t); // t_0 = t
		copyPolynomial(t, temp); // t = temp
		multiplyPolynomial(tempProduct, s, q); // sq


		addPolynomial(temp, s_0, tempProduct); // temp = s0 - sq				
		copyPolynomial(s_0, s); //s_0 = s
		copyPolynomial(s, temp); // s = temp
		copyPolynomial(a_0, b_0); // a_0 = b_0 
		copyPolynomial(b_0, r); // b_0 = r
		euclideanAlgorithm(q, r, a_0, b_0);
	}
}

char *int2bin(unsigned integer)
{
	int n = 3;
	char *binary = (char *)malloc(n+1);
	for(int i = 0; i < n; i++)
	{
		binary[i] = (integer & (int)1 << (n-i-1)) ? '1' : '0';		
	}
	binary[n] = '\0';
	return binary;
}

void powerPolynomial(byte result[ARRAY_SIZE], byte a[ARRAY_SIZE], int n)
{
	byte tempResult[ARRAY_SIZE];
	copyPolynomial(result, a);
	for(int i = 0; i < n-1; i++)
	{
		multiplyPolynomial(tempResult, result, a);
		copyPolynomial(result, tempResult);
	}
}

// 日本学家基于费马定理的求乘法逆算法
void Itoh_Tsujii_algo(byte inverseOfX[ARRAY_SIZE], byte x[ARRAY_SIZE])
{
	
	int powerOf2[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
	int k = 7; // for GF(2^131), n = 131 - 1 = 130, n = 2^7 + 2
	int a[] = {1,0,0,0,0,0,1,0};			
	byte next_x[ARRAY_SIZE], x_i[ARRAY_SIZE];	
	int i;
	copyPolynomial(x_i, x);
	for(i = 0; i <= k; i++)
	{
		int n_i = powerOf2[i];
	
		for(int j = 1; j <= i; j++)
		{			
			n_i += powerOf2[i-j]*a[j];		
		}
		int powerIndex = 1;
		for(int l = 0; l < n_i; l++)
		{
			powerIndex *= 2;
		}
		powerIndex += 1;
		// x^(2^(n_i) + 1)
		powerPolynomial(next_x, x_i, powerIndex);			
		if(a[i] == 1)
		{
			byte tempPoly[ARRAY_SIZE];
			copyPolynomial(tempPoly, next_x);
			powerPolynomial(next_x, tempPoly, 2);
			copyPolynomial(tempPoly, next_x);
			multiplyPolynomial(next_x, tempPoly, x);
		}
		copyPolynomial(x_i, next_x);
		
	}
	multiplyPolynomial(inverseOfX, next_x, next_x);	  
}

int main()
{
	initPrimitivePolynomial();
	byte a[ARRAY_SIZE], b[ARRAY_SIZE];
	byte addResult[ARRAY_SIZE], multResult[ARRAY_SIZE];
	
	
	a[1] | (1 << 0);
	a[0] = a[0] | (1 << 6);
	a[0] = a[0] | (1 << 4);
	a[0] = a[0] | (1 << 2);		
	a[0] = a[0] | (1 << 1);	
	a[0] = a[0] | (1 << 0);

	b[0] = b[0] | (1 << 7);
	b[0] = b[0] | (1 << 1);
	b[0] = b[0] | (1 << 0);
	addPolynomial(addResult, a, b);
	multiplyPolynomial(multResult, a, b);
	
	printf("Add result: ");
	printPolynomial(addResult);
	printf("Multiply result: ");
	printPolynomial(multResult);	

	byte test[ARRAY_SIZE];
	memset(test, 0, sizeof(byte) * ARRAY_SIZE);
	test[0] = test[0] | (1 << 5);
	test[0] = test[0] | (1 << 4);
	test[0] = test[0] | (1 << 3);
	byte q[ARRAY_SIZE], r[ARRAY_SIZE],s[ARRAY_SIZE], t[ARRAY_SIZE], resulttest[ARRAY_SIZE];		
	extendedEuclidenaAlgorithm(s, t, primitivePolynomial, test);
	byte inverseOfX[ARRAY_SIZE];
	memset(inverseOfX, 0, sizeof(byte)*ARRAY_SIZE);
	Itoh_Tsujii_algo(inverseOfX, test);
	printf("\n");	
	printf("Inverse polynomial by extended euclidean algorithm: ");
	printPolynomial(t);
	printf("\n");
	printf("Inverse polynomial by Itoh-Tsujii method: ");
	printPolynomial(inverseOfX);
	printf("\n");
	
	return 0;
}
