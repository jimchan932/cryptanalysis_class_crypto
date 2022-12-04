#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct
{
	int array[3][3];
} Matrix;


// 计算 a mod 26
int mod26(int a)
{
	return a < 0 ? 26 + (a%26) : a% 26;
}

// 矩阵相乘
void mult(Matrix *result, Matrix *a, Matrix *b)
{
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			result->array[i][j] = 0;
			for(int k = 0; k < 3; k++)
			{
				result->array[i][j] =
					result->array[i][j] + a->array[i][k]*b->array[k][j];
			}
			result->array[i][j] = mod26(result->array[i][j]);
		}
									
	}
}

// 计算 xA, 当中 x 为行向量， A 为矩阵
void multRowVector(int *rowResultVector, int rowVector[3], Matrix *a)
{
	for(int i = 0; i < 3; i++)
		rowResultVector[i] = 0;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			rowResultVector[i] = rowResultVector[i] + rowVector[j]*a->array[j][i]; 
		}
		rowResultVector[i] = mod26(rowResultVector[i]);
	}

}

void init(Matrix *a)
{
	for(int i =0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			a->array[i][j] = 0;
								
								 
}

void copy(Matrix *a, int array[3][3])
{
	for(int i =0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			a->array[i][j] = array[i][j];
}

// 矩阵相减
void subtract(Matrix *result, Matrix *a, Matrix *b)
{
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			result->array[i][j] = mod26(a->array[i][j] - b->array[i][j]);
		}
	   
	}
}

// Function: encrypts plaintext based on key = (A, b) to ciphertext （加密函数）
void affineHillCipherEncrypt(char *ciphertext, char *plaintext, Matrix *key_matrixA, int key_rowVectorB[3])
{
    int plaintextLength = strlen(plaintext);
	int plaintextBlock[3];	
	int ciphertextBlock[3];
	
	for(int i = 0; i < plaintextLength; i+=3)
	{
		plaintextBlock[0] = plaintext[i] - 65;
		plaintextBlock[1] = plaintext[i+1] - 65;
		plaintextBlock[2] = plaintext[i+2] - 65;				
		multRowVector(ciphertextBlock, plaintextBlock, key_matrixA);	
		ciphertextBlock[0] = mod26(ciphertextBlock[0] + key_rowVectorB[0]);
		ciphertextBlock[1] = mod26(ciphertextBlock[1] + key_rowVectorB[1]);
		ciphertextBlock[2] = mod26(ciphertextBlock[2] + key_rowVectorB[2]);
		ciphertext[i] = ciphertextBlock[0] + 65; 
		ciphertext[i+1] = ciphertextBlock[1] + 65;  
		ciphertext[i+2] = ciphertextBlock[2] + 65; 
	}
}

// 矩阵相加
void add(Matrix *result, Matrix *a, Matrix *b)
{
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			result->array[i][j] = (a->array[i][j]+b->array[i][j]) % 26;
		}
	}
}

// 计算余因子
int cofactor(int row, int col, Matrix *a)
{
	int val = (a->array[(row+1)%3][(col+1)%3] * a->array[(row+2)%3][(col+2)%3] -
			   a->array[(row+1)%3][(col+2)%3] * a->array[(row+2)%3][(col+1)%3]);
    return val%26; 		
}

// 计算最大公因数
int gcdExtended(int a, int b, int *x, int *y)
{
	// Base Case
	if(a ==0)
	{
		*x = 0;
		*y = 1;
		return b;
	}
	int x1, y1;
	int gcd = gcdExtended(b%a, a, &x1, &y1);

	*x = y1- (b/a)*x1;
	*y = x1;
	return gcd;
}

// 计算模26下的乘法逆元 
int multiplicativeInverse(int a)
{
	int gcd, x, y;
	gcd = gcdExtended(mod26(a), 26, &x, &y);
	if(gcd != 1) return -1;
	return mod26(x);
}

// 找出矩阵的逆
void findInverse(Matrix *result, Matrix *a)
{
	int i, j;
	int determinant = 0, determinantInverse;

	for(i = 0; i < 3; i++)
	{
		determinant = determinant
			+ (a->array[0][i]*(a->array[1][(i+1)%3] * a->array[2][(i+2)%3] -
							   a->array[1][(i+2)%3] * a->array[2][(i+1)%3])); 
	}

	determinantInverse = multiplicativeInverse(determinant);
	printf("Determinant inverse, %d", mod26(determinant));
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			int adjVal = mod26(cofactor(i, j ,a));
			result->array[j][i] = mod26(determinantInverse * adjVal);
		}
	}	
}

// 打印矩阵
void printMatrix(Matrix *a)
{
	for(int i = 0; i < 3; i++)
	{
		printf("| ");
		for(int j = 0; j < 3; j++)
		{
			printf("%2d ", a->array[i][j]);
		}
		printf(" |\n");
	}
}

int main()
{   
	char plaintext[] = "ADISPLAYEDEQUATION";   // 明文
	char ciphertext[] = "DSRMSIOPLXLJBZULLM";  // 密文

	/*
	  plaintextMatrix1, plaintextMatrix2 就是明文矩阵 X1, X2
	  ciphertextMatrix1, ciphertextMatrx2 就是密文矩阵 Y1, Y2
	  
	  plaintextDiffMatrix, ciphertextDiffMatrix 分别就是 X2 - X1, Y2 - Y1 的相减结果
	  inversePlaintextDiffMatrix 就是 X2 - X1 的逆矩阵
	  keyMatrix 就是 密钥矩阵A 
	*/
	Matrix plaintextMatrix1, plaintextMatrix2,   
		ciphertextMatrix1, ciphertextMatrix2,
		plaintextDiffMatrix, ciphertextDiffMatrix;
	Matrix inversePlaintextDiffMatrix;
	Matrix keyMatrix;
	int i1, i2;
	i1 = 0;
	for(int i = 0; i < 3; i++)
	{		
		for(int j = 0; j < 3; j++)
		{
			plaintextMatrix1.array[i][j] = plaintext[i1] -65;
			ciphertextMatrix1.array[i][j] = ciphertext[i1] -65;
			i1++;
		}
	}
	i2 = 9;

	for(int i = 0; i < 3; i++)
	{	
		for(int j = 0; j < 3; j++)
		{
			plaintextMatrix2.array[i][j] = plaintext[i2]-65;
		
				ciphertextMatrix2.array[i][j] = ciphertext[i2]-65;
			i2++;
		}
	
	}
	printMatrix(&ciphertextMatrix2);
	// 计算 plaintextDiffMatrix, ciphertextDiffMatrix
	subtract(&plaintextDiffMatrix, &plaintextMatrix2, &plaintextMatrix1);
	subtract(&ciphertextDiffMatrix, &ciphertextMatrix2, &ciphertextMatrix1);
	
	
	printf("\ncipher");
	printMatrix(&ciphertextDiffMatrix);
	printf("\n");
	// 求矩阵 X2 - X1 的逆
	findInverse(&inversePlaintextDiffMatrix, &plaintextDiffMatrix);
	printMatrix(&inversePlaintextDiffMatrix);
	// 通过 A = (X2 - X1)' (Y2 - Y1) 得到密钥矩阵
	mult(&keyMatrix, &inversePlaintextDiffMatrix, &ciphertextDiffMatrix);
	printf("\nKey Matrix A = \n");
	printMatrix(&keyMatrix);

	// 在这里通过一个明文-密文 1x3的行向量对， 找出密钥行向量b 
	int rowVector_Ax[3], bRowVector[3];
	//计算行向量 xA 
	multRowVector(rowVector_Ax, plaintextMatrix1.array[2], &keyMatrix);

	// 因为 y = xA + b, 由 y - xA, 找出 b
	for(int i = 0; i < 3; i++)
	{
		bRowVector[i] = mod26(ciphertextMatrix1.array[2][i] - rowVector_Ax[i]);
	}
	printf("b row vector = [ ");
	for(int i = 0; i < 3; i++)
	{
		printf("%d ",bRowVector[i]);
	}
	printf("]");

	// 通过从使用我们的方法找出的密钥Key=(A,b)使用仿射Hill密码使用样例明文找出密文， 验证结果的正确性
	char testCiphertext[18];
	affineHillCipherEncrypt(testCiphertext, plaintext, &keyMatrix, bRowVector);
	printf("\n\nplaintext = %s\n", plaintext);
	printf("ciphertext = %s\n", ciphertext);
	
	printf("ciphertext using affine Hill cipher and key=(A, b) = %s", testCiphertext);

	return 0;
}
