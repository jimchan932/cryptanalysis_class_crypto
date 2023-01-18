#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <string.h>
#include "sha2.h"
//#include "sha1.h"
#define BASE 16

typedef struct 
{
	mpz_t p;
	mpz_t q;
	mpz_t alpha;
	mpz_t beta;
}public_key;

typedef struct 
{
	mpz_t a;
}private_key;


typedef struct 
{
	mpz_t hashValue;
	mpz_t gamma;
	mpz_t delta;
}dsa_signature;

void init_keys(public_key *ku, private_key *kp)
{	
	mpz_init_set_str(ku->p, "dbb3c7fd991af258f58d2de99923ab65dbc196dcc3c9cda610cd5b60d96164f2d26c84431dff6aa20caee90fe9716c991baf2aa17c124cd2ac5d3a87b71d12e0496a5c3e45b5862b5d3c5f0e4c21e941354a79c60b84933ea9a82c6e7d65c729dc79b29f5fcdac1539f1cd082705853c8a88c0eb9a29683409effcd8abc3edec501fd0441791c10504de4ba5ee4152478a114574307598017222f3141a22be6306e6336d044f4229c7610ec6d97ca06ca5f488514114e2f41d8666fe62e7756b0928577523c88c9211c1c9afc708cb5593554ac47142a1b754b57e6661312780b5140c721589aef552c9b5ace3f459e2d59d5d1154a66917ed0e5d55b278bf91", BASE);
	mpz_init_set_str(ku->q, "f93802bf879afda83dc9f8b49f12112be93dfc0a5028a4e2ba810aeed9fc4867", BASE);
	mpz_init_set_str(ku->alpha, "0c0c5c14dc279396da528916e2d46b86afbfaa88750c0e6f912bf097227e5f0c86788001a7ca1c19a79fb45b6985b140b8187846d31bb4f9084bd07803a9f681afbd1d974a6bc7cc10e494df72a6c8d26b23968b0eef45b1e446f88f66549ff750d760c09b1e6a58031607adc0a65d4132bd3b452f1691d3dba17c48c1e3baa2e6f29266a66d5c4ffb8cf940b7ab48027f31afdc749a9a89e7699d861349d27cdbbd1d09e18a873050457aff334b45fbeaf2eecf350f9925fed927b7cf3dfc53c917f93fb58ea049934edc87fddf86e6290d7a453b27401c65777593d76904ef9fe0e233f8ecc20a624e4bd28c5571e6a1ff5676fcc9282fff8dc026cd1fbaea", BASE);
	gmp_printf("p: %Zd\nq: %Zd\nalpha: %Zd", ku->p, ku->q, ku->alpha);
	gmp_randstate_t random_state;
	mpz_init(kp->a);
	gmp_randinit_mt(random_state);
	mpz_urandomm(kp->a, random_state, ku->q);
	mpz_t zero;
	mpz_init_set_str(zero, "0", 10);
    while(mpz_cmp(kp->a, zero) == 0) // if a == 0 repeat rand gen
	{
		mpz_clear(kp->a);
		mpz_init(kp->a);
		mpz_urandomm(kp->a, random_state, ku->q);
	}
	mpz_clear(zero);
	gmp_printf("\na: %Zd\n", kp->a);
	mpz_init(ku->beta);
	mpz_powm(ku->beta, ku->alpha, kp->a, ku->p);
	mpz_t(test);
	mpz_init(test);
	mpz_powm(test, ku->alpha, kp->a, ku->p);
	gmp_printf("Beta: %Zd\n", ku->beta);
}

#define SHA256_HEX_OUT 64

void c2h(unsigned char *hexString, unsigned char *charArray, int len)
{
	char hex[16] = "0123456789abcdef";

	for(int i = 0; i < len; i++)
	{
		
		hexString[2*i+1] = hex[charArray[i] & 0x0f];
		hexString[2*i] = hex[charArray[i] >> 4];
	}
	hexString[2*len] = '\0';
	
}
void clear(dsa_signature *sign, public_key *ku, private_key *kp)
{
	mpz_clear(sign->gamma);
	mpz_clear(sign->delta);
	mpz_clear(ku->p);
	mpz_clear(ku->q);
    mpz_clear(ku->alpha);
	mpz_clear(ku->beta);
	mpz_clear(kp->a);
}

void dsa_sign(dsa_signature *sign, public_key public, private_key private,
			  unsigned char * message, unsigned int message_len,  mpz_t k)
// k is random number for each signature
{
	//计算gamma
	mpz_init(sign->gamma);	
	mpz_powm(sign->gamma, public.alpha, k,public.p);
	mpz_mod(sign->gamma, sign->gamma, public.q);

	// 计算delta
	sha256_context ctx;
	unsigned char *hash = NULL;	
	sha256_initContext(&ctx);
	sha256_update(&ctx, message, message_len);
	sha256_digest(&ctx, &hash);	
	hash[SHA256_OUT] = '\0';
    unsigned char hex_str_hash[SHA256_HEX_OUT];
	c2h(hex_str_hash, hash, SHA256_OUT);
	hex_str_hash[SHA256_HEX_OUT] = '\0';

	mpz_init(sign->hashValue);
	mpz_init_set_str(sign->hashValue, hex_str_hash, 16);
	mpz_init(sign->delta);
	mpz_set(sign->delta, sign->hashValue);

	mpz_addmul(sign->delta, private.a, sign->gamma);
	mpz_t inverse_k;
	mpz_init(inverse_k);
	mpz_invert(inverse_k, k, public.q);
	mpz_mul(sign->delta, sign->delta, inverse_k);
	mpz_mod(sign->delta, sign->delta, public.q);
	free(hash);	
}

int dsa_verify(dsa_signature sign, public_key public)
{
	mpz_t e_1, e_2, inverse_delta;
	mpz_init(e_1); mpz_init(e_2); mpz_init(inverse_delta);
	mpz_invert(inverse_delta, sign.delta, public.q);
	mpz_mul(e_1, sign.hashValue, inverse_delta);
	mpz_mod(e_1, e_1, public.q);
	mpz_mul(e_2, sign.gamma, inverse_delta);
	mpz_mod(e_2, e_2, public.q);

	//verification
	mpz_t temp1, temp2, temp_result;
	mpz_init(temp1); mpz_init(temp2); mpz_init(temp_result);
	mpz_powm(temp1, public.alpha, e_1, public.p);
	mpz_powm(temp2, public.beta, e_2, public.p);
	mpz_mul(temp_result, temp1, temp2);
	mpz_mod(temp_result, temp_result, public.p);
	mpz_mod(temp_result, temp_result, public.q);
	mpz_clear(temp1); mpz_clear(temp2); 
	if(mpz_cmp(temp_result, sign.gamma) == 0)
	{
		mpz_clear(temp_result);
		return 1;
	}
	else
	{
		mpz_clear(temp_result);
		return 0;
	}
}

int main()
{
	public_key ku; private_key kp;
	dsa_signature signature;
	init_keys(&ku, &kp);
	
	unsigned char *message = "Sysu20337262yuqingchao2022";
	int message_len = strlen(message);
	
	mpz_t k;
	mpz_init(k);
	mpz_set_ui(k, 12345);

	int hex_message_len = 53;
	unsigned char hex_message[53];	
	c2h(hex_message, message, message_len);
	dsa_sign(&signature, ku, kp, hex_message, hex_message_len, k);

	gmp_printf("\nSignature: (Gamma, Delta) = (%Zd, %Zd)\n", signature.gamma, signature.delta);
	
	int flag = dsa_verify(signature, ku);

	if(flag)
	{
		printf("\nVerification Success.\n");
	}
	else
		printf("\nVerification Failed.\n");	

	clear(&signature, &ku, &kp);
	return 0;
}
