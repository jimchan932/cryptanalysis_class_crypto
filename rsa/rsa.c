#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gmp.h>
#include <time.h>
#include "sha2.h"

#define BASE 16

#define MSG_BYTE_LEN 128
typedef struct {
    mpz_t n; /* Modulus */
    mpz_t e; /* Public Exponent */
} public_key;

typedef struct {
    mpz_t n; /* Modulus */
    mpz_t e; /* Public Exponent */
    mpz_t d; /* Private Exponent */
    mpz_t p; /* Starting prime p */
    mpz_t q; /* Starting prime q */
	mpz_t phi;
} private_key;

void print_hex(char* arr, int len)
{
    int i;
    for(i = 0; i < len; i++)
        printf("%02x", (unsigned char) arr[i]); 
}

void gen_keys(private_key* ku, public_key* kp)
{
	mpz_set_str(ku->p, "6435f4d9a8b2caeca0b56b48bc4316a88c19ac086738f8d051912a63682432b196b877bc25c311d1e89e94a8f0e4808382416a628971d43598aa7011aa76fad97d449e35c378b4e704b20972835c528c61c50138c85d934d140ff086466a7776d65fa4440d5e90afa7f641c8efca37b2547969a17802f814c1b52dad710567bf",BASE);
	mpz_set_str(ku->q, "c54c272fcaf1071e6f7277b85fe3463e1731b1a5d577b5b637ab37a25a3d03fff3ce8ec40e2735c10d8e9458b1894c985c473b2903af386cb38a46f24dfa735a13a43b3f138aa888131f29a93856328314482c2f7428d9a265359e3dc048b7d68b2e9da2b774c9c357058ba2950485c756c42fc848689660c6901c6cc2340ce7", BASE);
	mpz_mul(ku->n, ku->p, ku->q);

    mpz_t phi; mpz_init(phi);
    mpz_t tmp1; mpz_init(tmp1);
    mpz_t tmp2; mpz_init(tmp2);

	mpz_set_str(ku->e, "20337029", 10); // next prime after 20337021

    /* Compute ku->phi(n) = (p-1)(q-1) */
    mpz_sub_ui(tmp1, ku->p, 1);
    mpz_sub_ui(tmp2, ku->q, 1);
    mpz_mul(ku->phi, tmp1, tmp2);

    /* Calculate d (multiplicative inverse of e mod ku->phi) */	
	if(mpz_invert(ku->d, ku->e, ku->phi) == 0)
    {
        mpz_gcd(tmp1, ku->e, ku->phi);
        printf("gcd(e, ku->phi) = [%s]\n", mpz_get_str(NULL, 16, tmp1));
        printf("Invert failed\n");
	}
	
	mpz_set(kp->e, ku->e);
    mpz_set(kp->n, ku->n);
	
}
void block_encrypt(mpz_t C, mpz_t M, public_key kp)
{
    /* C = M^e mod n */
    mpz_powm(C, M, kp.e, kp.n); 
    return;
}

void block_decrypt(mpz_t M, mpz_t C, private_key ku)
{
	/* M = C^d mod n */
    mpz_powm(M, C, ku.d, ku.n); 
    return;
}

void hexStringToCharArray(unsigned char charArray[MSG_BYTE_LEN], unsigned char hexStr[256])
{
	for(int i = 0; i < MSG_BYTE_LEN; i++)
	{
		charArray[i] = (unsigned char)(hexStr[2*i] * 16 + hexStr[2*i+1]);
	}
}

void hexStringToCharArrayWithLen(unsigned char *charArray, unsigned char *hexStr, int n)
{
	charArray = malloc(sizeof(unsigned char)*(n/2));
	for(int i = 0; i < n; i += 2)
	{
		charArray[i] = (unsigned char)(hexStr[i] * 16 + hexStr[i+1]);
	}
}


void mgf(unsigned char result[MSG_BYTE_LEN], unsigned char m[MSG_BYTE_LEN])
{
	unsigned char temp[129];
	sha256_context ctx;
	memcpy(temp, m, MSG_BYTE_LEN*sizeof(unsigned char));

	for(int i = 0; i< 4; i++)
	{
		unsigned char *hashVal = NULL;
		temp[MSG_BYTE_LEN] = (char)i;
		sha256_initContext(&ctx);
		sha256_update(&ctx, temp, 129);
		sha256_digest(&ctx, &hashVal);
		memcpy(result + i * 32, hashVal, 32*sizeof(unsigned char));
		free(hashVal);
	}
}

void xor_(unsigned char result[MSG_BYTE_LEN], unsigned char a[MSG_BYTE_LEN], unsigned char b[MSG_BYTE_LEN])
{
	for(int i = 0; i < MSG_BYTE_LEN; i++)
	{
		result[i] = a[i] ^ b[i];
	}
}
void oaep_encode(unsigned char encoded_msg[256], mpz_t pad_plaintext, int msg_len ,unsigned char r_16[256])
{
	// encoding
	unsigned char r[MSG_BYTE_LEN];
	hexStringToCharArray(r, r_16);	
	unsigned char P1[MSG_BYTE_LEN], P2[MSG_BYTE_LEN];
	unsigned char X[MSG_BYTE_LEN], Y[MSG_BYTE_LEN];
	mgf(P1, r);
    mpz_t P1_16, P2_16, r_num_16;
	mpz_init_set_str(P1_16, P1, BASE);
	mpz_xor(P1_16, P1_16, pad_plaintext);
	mpz_get_str(X, BASE, P1_16);

	
	mgf(P2, X);
	mpz_init_set_str(P2_16, P2, BASE);
	mpz_init_set_str(r_num_16, r, BASE);
	mpz_xor(P2_16, P2_16, r_num_16);
	mpz_get_str(Y, BASE, P2_16);
	memcpy(encoded_msg, X, MSG_BYTE_LEN * sizeof(unsigned char));
	memcpy(encoded_msg+ MSG_BYTE_LEN*sizeof(unsigned char), Y, MSG_BYTE_LEN*sizeof(unsigned char));
}

void oaep_decode(unsigned char result[MSG_BYTE_LEN], unsigned char encrypted_msg[256])
{
	unsigned char X[MSG_BYTE_LEN];
	unsigned char Y[MSG_BYTE_LEN];
	unsigned char K[MSG_BYTE_LEN];
	unsigned char H[MSG_BYTE_LEN];
	unsigned char G[MSG_BYTE_LEN];
	memcpy(X, encrypted_msg, sizeof(unsigned char)*MSG_BYTE_LEN);
	memcpy(Y, encrypted_msg+MSG_BYTE_LEN, sizeof(unsigned char)*MSG_BYTE_LEN);
	mgf(H, X);
	xor_(K, H, Y);
	mgf(G, K);
	xor_(result, X, G);
}

int main()
{
    int i;
    mpz_t M;  mpz_init(M);
    mpz_t C;  mpz_init(C);
    mpz_t DC;  mpz_init(DC);
    private_key ku;
    public_key kp;

    // Initialize public key
    mpz_init(kp.n);
    mpz_init(kp.e); 
    // Initialize private key
    mpz_init(ku.n); 
    mpz_init(ku.e); 
    mpz_init(ku.d); 
    mpz_init(ku.p); 
    mpz_init(ku.q); 

    gen_keys(&ku, &kp);

    printf("---------------Private Key-----------------");
    printf("kp.n is [%s]\n", mpz_get_str(NULL, 16, kp.n));
    printf("kp.e is [%s]\n", mpz_get_str(NULL, 16, kp.e));
    printf("---------------Public Key------------------");
    printf("ku.n is [%s]\n", mpz_get_str(NULL, 16, ku.n));
    printf("ku.e is [%s]\n", mpz_get_str(NULL, 16, ku.e));
    printf("ku.d is [%s]\n", mpz_get_str(NULL, 16, ku.d));
    printf("ku.p is [%s]\n", mpz_get_str(NULL, 16, ku.p));
	//  printf("ku.q is [%s]\n", mpz_get_str(NULL, 16, ku.q));
    printf("ku.phi is [%s]\n", mpz_get_str(NULL, 16, ku.phi));	
	mpz_t pad_plaintext;
	printf("My name is chenjunming.\n");
	// unsigned char *plaintext_message  = "6368656E6A756E6D696E67";
	// int plaintext_message_len = strlen(plaintext_message);
	mpz_init_set_str(pad_plaintext, "6368656E6A756E6D696E67", BASE);
	unsigned char r_16[256] = "325585840509505d7234051d89edd2cfef333aabbb70f5b0c4da27a407e2ab259fc224e90b099226cecbbadd1eb81d044428ed8e0260debde0bc28e6aea483f7d2475187272099aa38719252f84f10cdb111b1b2955424f045ea55d952147fa8be91c415111f5c58de7026363209f5b66c24e8d2fab93545bd2306759437906c"; 

	printf("Message: 6368656E6A756E6D696E67\n\n");

	
	// encoding
	
	unsigned char encoded_msg[256], decoded_msg[MSG_BYTE_LEN];	
	
	unsigned char r[MSG_BYTE_LEN];
	hexStringToCharArray(r, r_16);	
	unsigned char P1[MSG_BYTE_LEN], P2[MSG_BYTE_LEN];
	unsigned char X[MSG_BYTE_LEN], Y[MSG_BYTE_LEN];
	mgf(P1, r);
    mpz_t P1_16, P2_16, r_num_16;
	mpz_init_set_str(P1_16, P1, BASE);
	mpz_xor(P1_16, P1_16, pad_plaintext);
	mpz_get_str(X, BASE, P1_16);

	
	mgf(P2, X);
	mpz_init_set_str(P2_16, P2, BASE);
	mpz_init_set_str(r_num_16, r, BASE);
	mpz_xor(P2_16, P2_16, r_num_16);
	mpz_get_str(Y, BASE, P2_16);
	memcpy(encoded_msg, X, MSG_BYTE_LEN * sizeof(unsigned char));
	memcpy(encoded_msg+ MSG_BYTE_LEN*sizeof(unsigned char), Y, MSG_BYTE_LEN*sizeof(unsigned char));
	
	
	// encryption
	mpz_init_set_str(M, encoded_msg, BASE);
	block_encrypt(C, M, kp);

	// decryption
    unsigned char decrypt_msg[MSG_BYTE_LEN];		
	block_decrypt(DC, C, ku);
	mpz_get_str(decrypt_msg, BASE, DC);	

	// decoding 
	unsigned char dec_P1[MSG_BYTE_LEN], dec_P2[MSG_BYTE_LEN], dec_r[MSG_BYTE_LEN], result;
	mpz_t dec_P1_16, dec_P2_16, dec_r_num_16;
	mgf(dec_P1, decrypt_msg);
   
	mpz_init_set_str(dec_P1_16, dec_P1, BASE); // 
	mpz_xor(dec_r_num_16, dec_P1_16, P2_16); // XORed result
	mpz_get_str(dec_r, BASE, dec_r_num_16); // random r
	mgf(dec_P2, dec_r);
	mpz_init_set_str(dec_P2_16, dec_P2, BASE);
	mpz_xor(dec_P2_16, dec_P2_16, P1_16);		

	mpz_get_str(decoded_msg, BASE, dec_P2_16);
	
	printf("\n\nDecrypted message: ");
	for(int i = 0; i < 22; i++)
	{
		printf("%c", decoded_msg[i]);
	}

    return 0;
}
