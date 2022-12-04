#include "cmathematics.h"
#include <stdio.h>
void printUintArray(unsigned int *arr, unsigned int noElements)
{
	printf("[");
	for(unsigned int i = 0; i < noElements; i++)
	{
		printf("%d", arr[i]);
	}
	printf("]\n");
}

void reverseArray(unsigned char *arr, int i, int f)
{
	while(i < f)
	{
		// swap corresponding elemnts accros the middle
		unsigned char tmp = arr[i];
		arr[i] = arr[f];
		arr[f] = tmp;
		i++;
		f--;
	}
}

void leftRotate(unsigned char *arr, int d, int n)
{
	reverseArray(arr, 0, d-1);
	reverseArray(arr, d, n-1);
	reverseArray(arr, 0, n-1);	
}

void rightRotate(unsigned char *arr, int d, int n)
{
	leftRotate(arr, n-d, n);
}
