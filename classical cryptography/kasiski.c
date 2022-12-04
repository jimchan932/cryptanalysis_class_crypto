#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_LEN 307
#define KEY_LEN 6
char encryptedMessage[] = "KCCPKBGUFDPHQTYAVINRRTMVGRKDNBVFDETDGILTXRGUDDKOTFMBPVGEGLTGCKQRACQCWDNAWCRXIZAKFTLEWRPTYCQKYVXCHKFTPONCQQRHJVAJUWETMCMSPKQDYHJVDAHCTRLSVSKCGCZQQDZXGSFRLSWCWSJTBHAFSIASPRJAHKJRJUMVGKMITZHFPDISPZLVLGWTFPLKKEBDPGCEBSHCTJRWXBAFSPEZQNRWXCVYCGAONWDDKACKAWBBIKFTIOVKCGGHJVLNHIFFSQESVYCLACNVRWBBIREPBBVFEXOSCDYGZWP";

float alphabetFrequency[] = {0.082, 0.015, 0.028, 0.043, 0.127, 0.022 ,0.020, 0.061, 0.070, 0.002,
	                           0.008, 0.040, 0.024, 0.067, 0.075, 0.019, 0.001, 0.060, 0.063, 0.091,
							   0.028, 0.010, 0.023, 0.001, 0.020, 0.001};
const float indexOfCoincidence = 0.065;

float absDiff(float a, float b)
{
	return a < b ? b-a : a-b; 
}

int mod26(int a)
{
	return a < 0 ? (26 - a) % 26 : a % 26;
}

float getIncidenceOfCoincidence(int frequencyList[26])
{
	float sum = 0;
	for(int i = 0; i < 26; i++)
		sum += (frequencyList[i] * (frequencyList[i]-1));
	return sum / (MESSAGE_LEN*(MESSAGE_LEN-1));
}

void getKeyLenByIncidenceOfCoincidence()
{
	int frequencyListWithKeyLen2[2][26];
	int frequencyListWithKeyLen3[3][26];
	int frequencyListWithKeyLen6[6][26];

	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 26; j++)
			frequencyListWithKeyLen2[i][j] = 0;
	}
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 26; j++)
			frequencyListWithKeyLen3[i][j] = 0;
	}	
	for(int i = 0; i < 6; i++)
	{
		for(int j = 0; j < 26; j++)
			frequencyListWithKeyLen6[i][j] = 0;
	}	
	for(int i = 0; i < MESSAGE_LEN; i++)
	{	   
		frequencyListWithKeyLen2[i%2][encryptedMessage[i]-65]++;
		frequencyListWithKeyLen3[i%3][encryptedMessage[i]-65]++;
		frequencyListWithKeyLen6[i%6][encryptedMessage[i]-65]++;		
	}

	printf("Incidence of coincidence with keylength = 2: ");
	for(int i = 0; i < 2; i++)
	{
		float ic = 0.0;
		for(int j = 0; j < 26; j++)
		{
			ic += (frequencyListWithKeyLen2[i][j]*
				   (frequencyListWithKeyLen2[i][j]-1));
		}
		ic /= (MESSAGE_LEN*(MESSAGE_LEN-1));
		printf("%f ", ic);
	}
	printf("\n");
	printf("Incidence of coincidence with keylength = 3: ");
	for(int i = 0; i < 3; i++)
	{
		float ic = 0.0;
		for(int j = 0; j < 26; j++)
		{
			ic += (frequencyListWithKeyLen3[i][j]*
				   (frequencyListWithKeyLen3[i][j]-1));
		}
		ic /= (MESSAGE_LEN*(MESSAGE_LEN-1));
		printf("%f ", ic);
	}
	printf("\n");
	printf("Incidence of coincidence with keylength = 6: ");
	for(int i = 0; i < 6; i++)
	{
		float ic = 0.0;
		for(int j = 0; j < 26; j++)
		{
			ic += (frequencyListWithKeyLen6[i][j]*
				   (frequencyListWithKeyLen6[i][j]-1));
		}
		ic /= (MESSAGE_LEN*(MESSAGE_LEN-1));
		printf("%f ", ic);
	}
	printf("\n");
}

int getKeyNearestIC(float mutualICList[26])
{
	int alphabetIndex = 0;
	float diff = absDiff(indexOfCoincidence, mutualICList[0]);
	for(int i = 1; i < 26; i++)
	{
		float temp = indexOfCoincidence - mutualICList[i];
		if(temp < 0) continue;
		if(diff > temp)
		{
			diff = temp;
			alphabetIndex = i;
		} 
	}
	return alphabetIndex;
}

float getMutualIndexOfCoincidence(char segmentedEncryptedMessage[52][KEY_LEN], int keyIndex, int key)
{
	int frequencyList[26];

	for(int i = 0; i < 26; i++)
	{
		frequencyList[i] = 0;
	}
	
	for(int i = 0; i < 51; i++)
	{
		frequencyList[mod26((int)(segmentedEncryptedMessage[i][keyIndex]-65) - key)]++;
	}
	if(keyIndex == 0)
		frequencyList[mod26((int)(segmentedEncryptedMessage[52][0]-65) - key)]++;

	int numberOfSegments = MESSAGE_LEN / KEY_LEN;

	float mutualIndex = 0.0f;
	for(int i = 0; i < 26; i++)
	{
		mutualIndex += alphabetFrequency[i] * frequencyList[i];
	}
	mutualIndex /= (MESSAGE_LEN * numberOfSegments);
	return mutualIndex;
}

typedef struct 
{
	char trigram[3];
	int numOccur;
	int distance;
} occurence;


occurence occurenceList[300];
int numOfOccurences = 0;
int compare(int startIndexA, int startIndexB)
{
	return (encryptedMessage[startIndexA] == encryptedMessage[startIndexB])
		&& (encryptedMessage[startIndexA+1] == encryptedMessage[startIndexB+1])
		&& (encryptedMessage[startIndexA+2] == encryptedMessage[startIndexB+2]);
}

int numOfRecurrence = 0;


void findOccurence()
{
	for(int i = 0; i < MESSAGE_LEN; i++)
	{		
		for(int j = i+3; j < MESSAGE_LEN; j++)
		{
			if(compare(i, j) && ((j-i) % 2 == 0)) // 比较长度为三的字段是否相等
			{
				
				strncpy(occurenceList[numOfOccurences++].trigram,
						encryptedMessage + i, 3);
				occurenceList[numOfOccurences].numOccur = 1;			
				occurenceList[numOfOccurences].distance = j-i;
				
				break;								
			}
		}
	}
}

void printHistogram()
{
	char alphabets[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int frequency[26];
	for (int i= 0; i < 26; i++)
	{
		int numOccur = 0;
		for(int j = 0; j < MESSAGE_LEN; j++)
		{
			if(encryptedMessage[j] == alphabets[i])
			{
				numOccur++;
			}
		}
		frequency[i] = numOccur;
	}
	printf("-----------Histogram----------------\n");
	for(int i = 0; i < 26; i++)
	{
		printf("%d: %c: ", i, alphabets[i]);
		for(int j = 0; j < frequency[i]; j++)
		{
			printf("*");
			
		}
		printf("%d\n", frequency[i]);
	}
}

int getDecryptedCharBySubtraction(int key, int encryptedChar)
{
	int diff = (encryptedChar -65) - (key-65);
	return diff < 0 ? 26 + diff : diff;
}
int add(int a, int b)
{
	return ((a- 65) + (b-65)) % 26;
}

void decryptMessageWithKey(char key[KEY_LEN], int key_len)
{
	for(int j = 0; j < MESSAGE_LEN; j += key_len)
	{
		printf("%c", getDecryptedCharBySubtraction(key[0], encryptedMessage[j]) + 65);
		printf("%c", getDecryptedCharBySubtraction(key[1], encryptedMessage[j+1]) + 65);
			
		printf("%c", getDecryptedCharBySubtraction(key[2], encryptedMessage[j+2]) + 65);
		printf("%c", getDecryptedCharBySubtraction(key[3], encryptedMessage[j+3]) + 65);
		printf("%c", getDecryptedCharBySubtraction(key[4], encryptedMessage[j+4]) + 65);
		printf("%c", getDecryptedCharBySubtraction(key[5], encryptedMessage[j+5]) + 65);
		printf(" ");	   
	}

}
int main()
{
	findOccurence();
	
	for(int i = 0; i < numOfOccurences; i++)
	{
		printf("trigram: %s, distance = %d\n", occurenceList[i].trigram, occurenceList[i].distance);
	}

	getKeyLenByIncidenceOfCoincidence(); // 打印重合指数

	char segmentedMessage[52][KEY_LEN];

	int index = 0;
	for(int i = 0; i < MESSAGE_LEN; i++)
	{
		if(i % KEY_LEN == 0) // segmentedMessage 把密文分开为长度为6的字段来储存
		{
			segmentedMessage[index][0] = encryptedMessage[i];
			segmentedMessage[index][1] = encryptedMessage[i+1];
			segmentedMessage[index][2] = encryptedMessage[i+2];
			segmentedMessage[index][3] = encryptedMessage[i+3];
			segmentedMessage[index][4] = encryptedMessage[i+4];
			segmentedMessage[index][5] = encryptedMessage[i+5];
			index++;
		}
	}
	segmentedMessage[52][1] = segmentedMessage[52][2]
		= segmentedMessage[52][3]
		= segmentedMessage[52][4]
		= segmentedMessage[52][5] = '#';  

	printf("Key guessed based on Kasiski method and mutual index of coincidence");
	char keyword[6];
	for(int keyLocation = 0; keyLocation < KEY_LEN; keyLocation++)
	{
		float mutualIndexList[26];
		for(int shiftAmount = 0; shiftAmount < 26; shiftAmount++)
		{
			mutualIndexList[shiftAmount] = getMutualIndexOfCoincidence(segmentedMessage, keyLocation, shiftAmount);
			
		}
		keyword[keyLocation] = getKeyNearestIC(mutualIndexList) + 65;
		printf(" %c", keyword[keyLocation]);
	}
	printf("\n\n");
	decryptMessageWithKey(keyword, 6);
	return 0;
}


