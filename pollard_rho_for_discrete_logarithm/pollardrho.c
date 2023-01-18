#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <gmp.h>

typedef struct {	
	mpz_t x;
	mpz_t a;
	mpz_t b;
} function_values;
	
// n = p-1
void function(function_values *val, mpz_t n, mpz_t p, mpz_t alpha, mpz_t beta)
{
	function_values result;
		
	unsigned int flag;
	mpz_t temp;

	mpz_init(temp);
	mpz_mod_ui(temp, val->x, 3);
	flag = mpz_get_ui(temp);
	// 分割 S1
	if(flag == 0)
	{
		mpz_mul_2exp(val->a, val->a, 1);
		mpz_mod(val->a, val->a, n);
		mpz_mul_2exp(val->b, val->b, 1);
		mpz_mod(val->b, val->b, n);
		mpz_mul(val->x, val->x, val->x);
		mpz_mod(val->x, val->x, p);	   
	}
	// 分割 S2	
	else if(flag == 1)
	{
		mpz_add_ui(val->a, val->a, 1);
		mpz_mod(val->a, val->a, n);
		mpz_mul(val->x, val->x, alpha);
		mpz_mod(val->x, val->x, p);
	}
	// 分割 S3	
	else
	{
		mpz_add_ui(val->b, val->b, 1);
		mpz_mod(val->b, val->b, n);
		mpz_mul(val->x, val->x, beta);
		mpz_mod(val->x, val->x, p);
	}
}

// g = alpha, y = beta
void pollard_dlp(mpz_t result, mpz_t alpha, mpz_t beta,mpz_t p,mpz_t n) 
{
	function_values val, val_prime;
	mpz_init(val.x);mpz_init(val_prime.x);
	mpz_init(val.a);mpz_init(val.b);
	mpz_init(val_prime.a);mpz_init(val_prime.b);
	
	mpz_set_ui(val.x,1); 
	mpz_set_ui(val.a,0);
	mpz_set_ui(val.b,0);
	//function(&val, n, p, alpha, beta);

	mpz_set(val_prime.x, val.x);
	mpz_set(val_prime.a, val.a);
	mpz_set(val_prime.b, val.b);
	//function(&val_prime, n, p, alpha, beta);

	int i = 1;
	unsigned int nVal = mpz_get_ui(n);
	for(int j = 1; j < nVal; j++)
	{
		unsigned int x, a, b, X, A, B;
		// 更新 (x, a, b)
		function(&val, n, p, alpha, beta);
		function(&val_prime, n, p, alpha, beta);
		function(&val_prime, n, p, alpha, beta);
		x = mpz_get_ui(val.x);		
		a = mpz_get_ui(val.a);
		b = mpz_get_ui(val.b);
		X = mpz_get_ui(val_prime.x);
		A = mpz_get_ui(val_prime.a);
		B = mpz_get_ui(val_prime.b);
		printf("%3u  %4u %3u %3u  %4u %3u %3u\n", i, x, a, b, X, A, B);
		i++;
		// 当 x == x' 的时候， 停止循环
		if(mpz_cmp(val.x, val_prime.x) == 0) break;
	}
	// 计算
	mpz_t aDiff, bDiff, bDiffInverse, gcd, gcd_aDiff_bDiff;
	mpz_init(aDiff);
	mpz_init(bDiff);
	mpz_init(bDiffInverse);
	mpz_init(gcd);
	mpz_init(gcd_aDiff_bDiff);

	// 求出 a' - a
	mpz_sub(aDiff, val.a, val_prime.a);
	// 求出 b' - b
	mpz_sub(bDiff, val.b, val_prime.b);   
	// 求出最大公因数 gcd (b' - b, n)
	mpz_gcd(gcd, bDiff, n);

	// pollard rho 算法失败情况
	if(mpz_cmp_ui(gcd, 1) != 1)
	{
		printf("Failure");
		return;
	}

	// 求出 (b' - b)^(-1)
	mpz_invert(bDiffInverse, bDiff, n);

	// 求出最大公因数 gcd(b' - b, a' - a)
	mpz_gcd(gcd_aDiff_bDiff, aDiff, bDiff);

	// 比较是否存在 乘法逆 (b' - b)^(-1)
	if(mpz_cmp_ui(bDiffInverse, 0) == 0)
	{
		// 如果不存在乘法逆但是 (b' - b) 能够整除 (a' - a)， 可以得到离散对数 
		if(mpz_cmp_ui(gcd_aDiff_bDiff,1) != 0 && mpz_cmp(aDiff, bDiff) > 0)
		{
			mpz_div(result, aDiff, bDiff);
		}
		else
		{
			printf("Failure no multiplicative inverse for b - b'");
			return;
		}
		
	}
	// 通过乘法逆 (b' - b)^(-1) 求出离散对数
	else
	{	
		mpz_mul(result, aDiff, bDiffInverse);
	}	

	mpz_clear(val.x); mpz_clear(val_prime.x);
	mpz_clear(val.a); mpz_clear(val_prime.a);
	mpz_clear(val.b); mpz_clear(val_prime.b);
	mpz_clear(aDiff); mpz_clear(bDiff); mpz_clear(bDiffInverse);
	mpz_clear(gcd); mpz_clear(gcd_aDiff_bDiff);
}

int main()
{	
	mpz_t alpha, beta, test_n, test_p, test_result;
	mpz_init(alpha);
	mpz_init(beta);
	mpz_init(test_n);
	mpz_init(test_p);
	mpz_init(test_result);
	mpz_init_set_str(alpha, "2", 10);
	mpz_init_set_str(beta, "5", 10);
	mpz_init_set_str(test_n, "1018", 10);
	mpz_init_set_str(test_p, "1019", 10);
	
   	pollard_dlp(test_result,alpha,beta,test_p,test_n);	
   	printf("Calculated discrete log = %u", mpz_get_ui(test_result));
	return 0;
}


