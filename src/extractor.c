#include <stdio.h>
#include <stdlib.h>
#include "extractor.h"
#include "skeleton.h"

void saveVectorToFile(float *data, int zoneAmount){
	FILE *f;
	int i;
	if((f=fopen("sans.txt", "a+")) == NULL){
		printf("Nie można otworzyć pliku");
		exit(1);
	}

	for(i=0; i < zoneAmount; i++){
		fprintf(f, "%f ", data[i]);	
	}
	
	fprintf (f, "\n");
	fclose(f);
}

float absf(float x){
	if(x<0)
		return (-1)*x;
	else
		return x;
}

int compareWithTestData(float *data, int zoneAmount){
	FILE *f;
	int lineLetter, eoLine; float deviation, minDeviation;
	float  pixelsInZoneFile;
	int const charsAmount = CHARS_AMOUNT;	
	int i,j;   //check amount of chars

	if((f = fopen("sans.txt", "r")) == NULL){
		printf("Cannot open file");
		exit(1);
	}
	
	minDeviation = 10000;
	for(i=0; i<charsAmount; i++){  
		deviation = 0;
		for(j=0; j<zoneAmount; j++){
			fscanf(f, "%f", &pixelsInZoneFile);
			deviation += absf(pixelsInZoneFile - data[j]);
			}
//		printf("deviation = %f \n", deviation);

		if(minDeviation > deviation){

			lineLetter = i;
			minDeviation = deviation;
			//printf("minDeviation: %f \n", minDeviation);
		}
	}
	fclose(f);	
	return lineLetter;					 
}

char charFromLine(int line){
	char bigLetters[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','r','s','t','u', 'w', 'x','y','z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'R', 'S', 'T', 'U', 'W', 'X', 'Y', 'Z'};

	return bigLetters[line];
}

char countPixelsInZone(int **bittab, int width, int height, int save){
	int i,j,k,l, beginX, beginY, endX, endY, pixelsInZone=0, totalAmount=0;
	const int N = ZONE_PER_LINE;
	float data[N*N];
	
	beginX = 0;
	endX = div(height, N).quot;

	for(k=0; k<N; k++){
		beginY = 0;
		endY = div(width, N).quot;
		//printf("%d", endY);
		for(l=0; l<N; l++){
			pixelsInZone = 0; 
			for(i=beginX; i<endX; i++)
				for(j=beginY; j<endY; j++){
					if(bittab[i][j] != 0)
						pixelsInZone++;
				}	
			beginY += div(width, N).quot;
			endY += div(width, N).quot;
			totalAmount += pixelsInZone;	
			data[N*k+l] = pixelsInZone;
		}
		beginX += div(height, N).quot;
		endX += div(height, N).quot;
	}
	if(totalAmount != 0)
		for(i=0; i<(N*N); i++)
			data[i] = data[i] / totalAmount;
	else{
		printf("No text to read");
		exit(1);
	}

	if(save == 1){
		saveVectorToFile(data, N*N);
	}
	else		
	//printf("%c\n", charFromLine(compareWithTestData(data, N*N)));	
		return charFromLine(compareWithTestData(data, N*N));
}	











