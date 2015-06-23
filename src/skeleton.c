#include <stdio.h>
#include <stdlib.h>
#include "LibBMP.h"



void fillBittab(int width, int height){
	int j,k;
	for(j=0; j<height+2; j++)
		for(k=0; k<width+2; k++){
			if(j>0 && k>0 && j<height+1 && k<width+1){
				if(white(k, j))	
					binaryPicture[k+1][j+1]=0;
				else
					binaryPicture[k+1][j+1]=1;
			}
			else 
				binaryPicture[k][j]=0;
		}

}

void writeTab(int width, int height){
	int k,j;

	for(j=0; j<height; j++){
		for(k=0; k<width; k++){
			printf("%d",binaryPicture[k][j]);
			}
	printf("\n");
	}	
}

void deletingChecked(int height, int width){
	int j,k;
	
	 for(j=1; j<=height+1; j++)
		for (k=1; k<=width+1; k++)
			if(binaryPicture[j][k] == 2) 
				binaryPicture[j][k] = 0;
}

void createSkeleton (int height, int width){
	int j,k;

	int changes = 1;
	fillBittab(width, height );
	while(changes){
		changes=0;
	/*checking border pixels*/		
	 	for(j=height; j>=1; j--)
			for (k=1; k<width+1; k++)
				if(binaryPicture[k][j] && maskA0(k,j)){ 
					binaryPicture[k][j] = 2;
				}
	/*deleting pixels with 3 sticked neighbours*/
	 	for(j=height; j>=1; j--)
			for (k=1; k<width+1; k++)
				if(binaryPicture[k][j]==2 && maskA1(k,j)){ 
					binaryPicture[k][j] = 0;
					changes = 1;
				}
	/*deleting pixels with 3,4 sticked neighbours*/
	 	for(j=height; j>=1; j--)
			for (k=1; k<width+1; k++)
				if(binaryPicture[k][j]==2 && maskA2(k,j)){ 
					binaryPicture[k][j] = 0;
					changes = 1;
				}
	 	
	/*deleting pixels with 3,4,5 sticked neighbours*/
		for(j=height; j>=1; j--)
			for (k=1; k<width+1; k++)
				if(binaryPicture[k][j]==2 && maskA3(k,j)){ 
					binaryPicture[k][j] = 0;
					changes = 1;
				}

	/*deleting pixels with 3,4,5,6 sticked neighbours*/
	 	for(j=height; j>=1; j--)
			for (k=1; k<width+1; k++)
				if(binaryPicture[k][j]==2 && maskA4(k,j)){ 
					binaryPicture[k][j] = 0;
					changes = 1;
				}

	/*deleting pixels with 3,4,5,6,7 sticked neighbours*/
	 	for(j=height; j>=1; j--)
			for (k=1; k<width+1; k++)
				if(binaryPicture[k][j]==2 && maskA5(k,j)){ 
					binaryPicture[k][j] = 0;
					changes = 1;
				}
	}	
	/*creating 1 px witdh skeleton*/
	 	for(j=height; j>=1; j--)
			for (k=1; k<width+1; k++)
				if(binaryPicture[k][j] && maskApix(k,j)){ 
					binaryPicture[k][j] = 0;
				}
	//writeTab(width, height);
}

int maskNumChecker(int x, int y, char flag){
	int sum=0, i, j;
	int checkMask[3][3] = {{128, 1, 2}, {64, 0, 4}, {32,16,8}};

	if (flag == 'V'){
		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
				checkMask[i][j] = 1;
		checkMask[1][1] =0;
	}

	for(i=-1; i<=1; i++)
		for(j=-1; j<=1; j++)
			if(binaryPicture[x+i][y+j]!=0)
				sum=sum+checkMask[i+1][j+1];
	//printf("%d \n", sum);
	return sum;		
}

int binarySearch(int *maskTab, int target, int n) {
	int first = 0; int last = n-1;
	int middle = (first+last)/2;   
while( first <= last )
   {
      if (maskTab[middle] < target )
         first = middle + 1;    
      else if (maskTab[middle] == target ) 
      {
         return 1;
      }
      else
         last = middle - 1;
 
      middle = (first + last)/2;
   }
   if ( first > last )
      return 0;
 }


int maskA0(int i, int j){
	int A0[] = {3, 6, 7, 12, 14, 15, 24, 28, 30, 31, 48, 56, 60, 62, 63, 96, 112, 120, 124, 126, 127, 129, 131, 135, 143, 159, 191, 192, 193, 195, 199, 207, 223, 224, 225, 227, 231, 239, 240, 241, 243, 247, 248, 249, 251, 252, 253, 254};
	return binarySearch(A0, maskNumChecker(i, j, 'S'),48);
}
		
int maskA1 (int i, int j){
	int A1[] = {7,14,28,56,112,131,193,224};
	return binarySearch(A1, maskNumChecker(i, j, 'S'),8);
}

int maskA2(int i, int j){
	int A2[] = {15,30,60,120,135,195,225,240};
	int qA1 = maskA1(i, j);
	return (qA1 + binarySearch(A2, maskNumChecker(i, j, 'S'),8));
}


int maskA3(int i, int j){
	int A3[] = {31, 62, 124, 143, 199, 227, 241, 248};
	int qA2 = maskA2(i, j);
	return (qA2 + binarySearch(A3, maskNumChecker(i, j, 'S'),8));
}

int maskA4(int i, int j){
	int A4[] = {63, 126, 159, 207, 231, 243, 249, 252};
	int qA3 = maskA3(i, j);
	return (qA3 + binarySearch(A4, maskNumChecker(i, j, 'S'),8));
}


int maskA5(int i, int j){
	int A5[] = {191, 239, 251, 254};
	int qA4 = maskA4(i, j);
	return (qA4 + binarySearch(A5, maskNumChecker(i, j, 'S'),4));
}

int maskApix(int i, int j){
	int Apix[] = {3, 6, 12, 24, 48, 96, 127, 192, 223, 241, 247, 253};
	int qA5 = maskA5(i, j);
	return (qA5 + binarySearch(Apix, maskNumChecker(i, j, 'S'),12));

}

