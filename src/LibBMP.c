#define _CRT_SECURE_NO_DEPRECATE

#include "LibBMP.h"
#include "skeleton.h"
#include "extractor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static int GetByte(FILE *fp);
static int GetWord(FILE *fp);
static int GetDoubleWord(FILE *fp);
int Loaded = 0;
int threadsCreated = 0;
int finished = 0;


/* This is used for storing file data for a BMP file loaded using the LoadBMPFile function */
static Colour internalBitmap[MAXH][MAXW];
static int loadSuccess = 0;



void LoadBMPFile(char* filename, int* width, int* height)
{
	Loaded = 1;
	/* For reading data from the file */
	int colourDepth, compression, fileSize;
	int x, y;

	/* Open BMP file */
	FILE *fptr;
	fptr = fopen(filename, "rb");
	if (fptr == NULL) {
		printf("Could not open input file: \"%s\"\n", filename);
		exit(EXIT_FAILURE);
	}

	/* Read file size */
	(void)fseek(fptr, 2, SEEK_SET);
	fileSize = GetDoubleWord(fptr);

	/* Read width and height of image */
	(void)fseek(fptr, 18, SEEK_SET);
	*width = GetDoubleWord(fptr);
	(void)fseek(fptr, 22, SEEK_SET);
	*height = GetDoubleWord(fptr);

	/* Read colour depth */
	(void)fseek(fptr, 28, SEEK_SET);
	colourDepth = GetWord(fptr);

	/* Read compression */
	(void)fseek(fptr, 30, SEEK_SET);
	compression = GetDoubleWord(fptr);

/* Ensure a 24-bit, and therefore uncompressed BMP file */
	if ((colourDepth != 24) || (compression != 0)) {
		printf("Not a valid 24-bit BMP file.\n");
		printf("Colour depth should be 24 - it is %d.\n", colourDepth);
		printf("Compression should be 0 - it is %d.\n", compression);
		exit(EXIT_FAILURE);
	}

	/* Read the pixel information */
	(void)fseek(fptr, 53, SEEK_SET);

	/* Ensure memory is adequate */
	if ((*height >= MAXH) || (*width >= MAXW)) {
		printf("Not enough memory for a %dx%d image.\n", MAXW, MAXH);
		printf("Increase the values of MAXW and MAXH in LibBMP.h\n");
		exit(EXIT_FAILURE);
	}

	for(y = *height; y >= 0; y--) {
		for(x = 0; x < *width; x++) {
			internalBitmap[y][x].blue = (byte)GetByte(fptr);
			internalBitmap[y][x].green = (byte)GetByte(fptr);
			internalBitmap[y][x].red = (byte)GetByte(fptr);

		  if(internalBitmap[y][x].red < 50)
                  	internalBitmap[y][x].red = 0;
		  else
                        internalBitmap[y][x].red = 255;

		  if(internalBitmap[y][x].blue < 50)
		       internalBitmap[y][x].blue = 0;
		  else
		       internalBitmap[y][x].blue = 255;

		  if(internalBitmap[y][x].green < 50)
		       internalBitmap[y][x].green = 0;
		  else
		       internalBitmap[y][x].green = 255; 
	
		}
		/* Realign the file pointer, scan lines are word aliged */
		(void)fseek(fptr, *width % 4, SEEK_CUR);
	}

	printf("Successfully loaded bitmap information:\n");
	printf("  - Filename: \"%s\"\n", filename);
	printf("  - Width: %d\n", *width);
	printf("  - Height: %d\n", *height);
	printf("  - Bytes read: %d\n\n", fileSize);
	loadSuccess = 1;
	(void)fclose(fptr);
}

/* channel 0, 1, 2 = red, green, blue */
int GetPixelValue(int row, int col, int channel)
{
	if (channel == 0)
		return internalBitmap[row][col].red;
	else if (channel == 1)
		return internalBitmap[row][col].green;
	else
		return internalBitmap[row][col].blue;
}

/* channel 0, 1, 2 = red, green, blue */
void SetPixelValue(int value, int row, int col, int channel)
{
	byte val = (byte)value;
	if (channel == 0)
		internalBitmap[row][col].red = val;
	else if (channel == 1)
		internalBitmap[row][col].green = val;
	else
		internalBitmap[row][col].blue = val;
}

void SaveBMPFile(char* filename, int width, int height,int top, int bottom)
{
	/* BMP file format header, for uncompressed 24-bit colour */
	byte header[54] = {	0x42,0x4D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0xC2,0x1E,0x00,0x00,0xC2,0x1E,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00 };

	FILE *fptr;
	byte* fileData;

	int aligned, padding, totalSize;
	int i;
	int x, y;
	int writeIndex = 55;

	/* Ensure a file has been loaded prior to this call */
	if (!loadSuccess) {
		printf("BMP file must be loaded before saving.\n");
		exit(EXIT_FAILURE);
	}

	/* Calculate total size of picture, taking into account padding */
	aligned = 4 - (width * 3) % 4;
	padding = (aligned == 4) ? 0 : aligned;
	totalSize = 54 + (((width * 3) + padding) * height)*2;

	/* Allocate room to store filedata */
	fileData = (byte*)malloc((size_t)totalSize);
	if (fileData == NULL) {
		printf("Could not allocate sufficient memory.\n");
		exit(EXIT_FAILURE);
	}

	/* Initialise first 54 bytes, set total size and width and height correctly */
	for (i = 0; i < 54; i++) {
		fileData[i] = header[i];
	}
	/* File size */
	fileData[2] = (byte)(totalSize & 0xFF);
	fileData[3] = (byte)((totalSize >> 0x08) & 0xFF);
	fileData[4] = (byte)((totalSize >> 0x10) & 0xFF);
	fileData[5] = (byte)((totalSize >> 0x18) & 0xFF);
	/* Width */
	fileData[18] = (byte)(width & 0xFF);
	fileData[19] = (byte)((width >> 0x08) & 0xFF);
	fileData[20] = (byte)((width >> 0x10) & 0xFF);
	fileData[21] = (byte)((width >> 0x18) & 0xFF);
	/* Height */
	fileData[22] = (byte)(height & 0xFF);
	fileData[23] = (byte)((height >> 0x08) & 0xFF);
	fileData[24] = (byte)((height >> 0x10) & 0xFF);
	fileData[25] = (byte)((height >> 0x18) & 0xFF);

	/* Copy bitmap data */
	for(y = bottom; y >= top; y--) {
		for(x = 0; x < width; x++) {
			fileData[writeIndex++] = internalBitmap[y][x].blue;
			fileData[writeIndex++] = internalBitmap[y][x].green;
			fileData[writeIndex++] = internalBitmap[y][x].red;
		}
		writeIndex += width%4;
	}

	/* Write to file */
	fptr = fopen(filename, "wb");
	if (fptr == NULL) {
		printf("Could not open output file: \"%s\"\n", filename);
		exit(EXIT_FAILURE);
	}
	
	/* Write data */
	(void)fwrite(fileData, 1, (size_t)totalSize, fptr);
	/* Close files */
	(void)fclose(fptr);


	/* Release memory */
	free(fileData);

	printf("Successfully saved bitmap information:\n");
	printf("  - Filename: \"%s\"\n", filename);
	printf("  - Width: %d\n", width);
	printf("  - Height: %d\n", height);
	printf("  - Bytes written: %d\n\n", totalSize);
}


static int GetByte(FILE *fp)
{
	int w;
	w =  (int) (fgetc(fp) & 0xFF);
	return(w);
}

static int GetWord(FILE *fp)
{
	int w;
	w =  (int) (fgetc(fp) & 0xFF);
	w |= ((int) (fgetc(fp) & 0xFF) << 0x08);
	return(w); }

static int GetDoubleWord(FILE *fp)
{
	int dw;
	dw =  (int) (fgetc(fp) & 0xFF);
	dw |= ((int) (fgetc(fp) & 0xFF) << 0x08);
	dw |= ((int) (fgetc(fp) & 0xFF) << 0x10);
	dw |= ((int) (fgetc(fp) & 0xFF) << 0x18);
	return(dw);
}

int notWhite(int x, int y){
	if(internalBitmap[y][x].blue != 255 && internalBitmap[y][x].red != 255 && internalBitmap[y][x].green != 255) return 1;
	return 0;
}

int white(int x, int y){
	if(internalBitmap[y][x].blue == 255 && internalBitmap[y][x].red == 255 && internalBitmap[y][x].green == 255) return 1;
	return 0;
}


void getColour(int x, int y){
	printf("%d\n",internalBitmap[y][x].blue);
	printf("%d\n", internalBitmap[y][x].red );
	printf("%d\n", internalBitmap[y][x].green);
}

Line getNextLine(int width, int height){

	static Line line;	
	if(Loaded == 0){
		puts("Load bitmap first");
		return line;
	}

	int x;
	int y;
	int height1 = 0;
	int height2;
	int blackFlag = 1;
	int blackPikselFound = 0;
	int lookForWhiteLine = 0;

	for(y = height; y >= 0; y--){
		blackPikselFound = 0;
		for(x = width - 1; x >= 0; x--){
			if(!blackFlag){
				lookForWhiteLine = 1;
				if(binaryPicture[x][y]){
					blackPikselFound = 1;
					x = 0;
				}
			}
			if(binaryPicture[x][y] && blackFlag ){
				height1 = y;
				line.startLine = y;
				blackFlag = 0;
				x = 0;
			}
		}
		if(lookForWhiteLine && !blackPikselFound && !blackFlag){
			height2 = y;
			line.endLine = y;
			y = 0;
		}
	}

	return line;
}

char compareText(int xb, int yb,int xx,int yy){
	int x,y,i;
	int **bittab;

	int h = xx-xb+2;
	int w = yy-yb+2;

	char letter;
	bittab = (int**)malloc(h*sizeof(int*));
	for(i=0; i<h; i++)
		bittab[i]=(int*)malloc(w*sizeof(int));

	for(y = yb; y < yy; y++) {
		for(x = xb; x < xx; x++) {
			if(!binaryPicture[x][y])
				bittab[x-xb][y-yb] = 0;
			else
				bittab[x-xb][y-yb] = 1;
			//printf("%d", bittab[x-xb][y-yb]);
		}
		//printf("\n");
	}
	letter = countPixelsInZone(bittab, w, h, save);	
	free(bittab);
	return letter;
}

void printInternalBitmap(int xb, int yb,int xx,int yy){
    int x,y;

    for(y = yb; y < yy; y++) {
                for(x = xb; x < xx; x++) {
			printf("%d",binaryPicture[x][y]);
                }
                puts(" ");
        }
	printf("\n");
}


void printInternalBitmap2(int xb, int yb,int xx,int yy){
    int x,y;

    for(y = yb; y < yy; y++) {
                for(x = xb; x < xx; x++) {
			if(white(x,y))
				printf("0");
			else
				printf("1");
                }
                puts(" ");
        }
	printf("\n");
}


Letter getNextLetter(int startX, int top, int width, int height,int *space){

	int w,h;
	Letter letter;
	letter.x = -1;
	letter.y = -1;
	letter.height = -1;
	letter.width = -1;
	int spaceParameter = 20;

	int x,y;
	int ok = 0;


	for(x = startX; x < width; x++){ 
		for(y = top; y < height; y++){
		 	if(binaryPicture[x][y]){
				ok = 1;
			}		
		}
	}

	if (ok == 0)  return letter;

	int black = 0;

	for(x = startX; x < width; x++){ 
		for(y = height; y >= top ; --y){
		 	if(binaryPicture[x][y] ){
				letter.x = x;
				y = 0;
				x = width;
			}		
		}
	}


	for(x = letter.x; x < width; x++){
		for(y = height; y >= top; --y){
			if(binaryPicture[x][y]){
				black = 1;
				y = 0;
			}
		}
		if(!black){
			letter.width = x - letter.x ;
			x = width;
		}
		black = 0;
	}


	for(y = height; y >= top ; --y){
		for(x = letter.x; x < letter.x + letter.width ; x++){
			if(binaryPicture[x][y]){
				letter.y = y;
				y = 0;
			}
		}
	}

	for(y = letter.y; y>= top; --y){
		for(x = letter.x; x < letter.x  + letter.width ; x++){
			if(binaryPicture[x][y]){
				black = 1;
				x = letter.width + letter.x;
			}
		}
		if(!black){
			letter.height = letter.y - y;
			y = 0;
		}
		black = 0;
	}

	int spaceCounter = 0;
	black = 0;
	for(x = letter.x + letter.width ; x < width; x++){
		for(y = height; y >= top; --y){
			if(binaryPicture[x][y]){
				black = 1;
				y = 0;
				x = width;
			}
		}
		if(!black){
			spaceCounter++;
		}
		black = 0;
	}
	
	if(spaceCounter >= spaceParameter) *space = 1;

	return letter;
}


void *extractLetters(void *argsVoid){
	int lettersInLine = 50;
	char line[100];
	int i;
	while(!threadsCreated){
		usleep(1000);
	}


	int space = 0;
	int startX = 0;
	int iterator = 0;
	CutBitmapArgs *args;
	args = (CutBitmapArgs*) argsVoid;
	int newHeight = args->heightBottom - args->heightTop;
	int keepGoing = 1;
	int width = args->originalWidth;
	int height = args->heightBottom;
	Letter letter;


	for(i = 0; i < 100; i++)
		line[i] = 0;

        while(keepGoing){
		space = 0;
                letter = getNextLetter(startX,args->heightTop,width,height,&space);
//		printf("x %d ",letter.x);
//		printf("y %d ",letter.y);
//		printf("h %d ",letter.height);
//		printf("w %d\n",letter.width);

                if(letter.x == -1 && letter.y == -1) 
			keepGoing = 0;
		else{
		line[iterator++] = compareText(letter.x ,letter.y + 1 -letter.height,letter.x + letter.width,letter.y +1);
		if(space) line[iterator++]=' ';
	}
		startX = letter.x + letter.width;

        }
	line[iterator] = '\n';
	pthread_exit(line);
}


void fillBinaryPicture(int width, int height){
	int i, j;
	width += 2;
	height += 2;

	binaryPicture = (int**)malloc(width*sizeof(int**));
	for(i=0; i<width; i++)
		binaryPicture[i] = (int*)malloc(height*sizeof(int*));
}

void extractText(char *filename){
	int j = 0;
	int originalWidth,originalHeight;
	CutBitmapArgs *argsPtr = malloc(sizeof(CutBitmapArgs));
	if(argsPtr == NULL){
		printf("Could not allocate sufficeint memory.\n");
		exit(EXIT_FAILURE);
	}
	
	char bitmap[16];

	Line line;
	LoadBMPFile(filename,&argsPtr->originalWidth, &argsPtr->originalHeight);
	//printInternalBitmap2(0,0,argsPtr->originalWidth,argsPtr->originalHeight);
	int mainHeight = argsPtr->originalHeight - 2;
	int lastStartLine = 0;
	int countLines = 0;
	line.startLine = 1;
	originalWidth = argsPtr->originalWidth;
	originalHeight = argsPtr->originalHeight;


	fillBinaryPicture(originalWidth, originalHeight);
	createSkeleton(originalHeight-2, originalWidth-2);
	

	while(lastStartLine != line.startLine){
		lastStartLine = line.startLine;
		line = getNextLine(argsPtr->originalWidth, mainHeight);	
		if(lastStartLine != line.startLine){
			countLines++;
			mainHeight = line.endLine;
			argsPtr->heightTop = line.endLine - 1;
			argsPtr->heightBottom = line.startLine;
		}
	}
	printf("\nilosc lini %d\n",countLines);


	pthread_t *Threads = malloc(sizeof(pthread_t)*(countLines+1));	
	if(Threads == NULL){
		printf("Could not allocate sufficeint memory.\n");
		exit(EXIT_FAILURE);
	}
	CutBitmapArgs *args =malloc(sizeof(CutBitmapArgs)*(countLines+1));
	if(args == NULL){
		printf("Could not allocate sufficeint memory.\n");
		exit(EXIT_FAILURE);
	}

	countLines = 0;
	line.startLine = 1;
	mainHeight = argsPtr->originalHeight - 2;
	lastStartLine = 0;

	while(lastStartLine != line.startLine){
		lastStartLine = line.startLine;
		line = getNextLine(argsPtr->originalWidth, mainHeight);	
		if(lastStartLine != line.startLine){
			sprintf(bitmap,"%d",countLines);
			countLines++;
			mainHeight = line.endLine;
			argsPtr->heightTop = line.endLine - 1;
			argsPtr->heightBottom = line.startLine;
			memcpy(argsPtr->filename, bitmap,16);
			args[countLines-1] = *argsPtr;
		}
	}

	//argsPtr->heightTop = -1;
	//args[countLines] = *argsPtr;
	for(j = 0; j < countLines  + 1; j++){
		if(pthread_create(&Threads[j],NULL,extractLetters,(void *)&args[j])){
			printf("Error while creating thread ");
			abort();
		}
	}

	threadsCreated = 1;

	int i = 0;
	void *status ;
	FILE *f;
        if((f=fopen("output.txt", "w+")) == NULL){
                printf("Nie można otworzyć pliku");
                exit(1);
        }

	for(i = countLines - 1; i>=0; i--){
		if(pthread_join(Threads[i],&status)){
			printf("Error while ending thread ");
			abort();
		};
		//printf(" %d %s",i, (char *)status);
		fprintf(f,"%s",(char *)status);
	}
	
	
	fclose(f);

	free(args);
	free(Threads);
	free(argsPtr);
}
