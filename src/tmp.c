#include <stdio.h>
#include <stdlib.h>

int **binaryPicture;

void fillBinaryPicture(int width, int height){
        int i, j;
        width += 2;
        height += 2;

        binaryPicture = (int**)malloc(width*sizeof(int**));
        for(i=0; i<width; i++)
                binaryPicture[i] = (int*)malloc(height*sizeof(int*));
}


void main (){
	int i,j;
	fillBinaryPicture(130, 30);
	for(i=0; i<130; i++)
		for(j=0; j<30; j++)
			binaryPicture[i][j];
}
