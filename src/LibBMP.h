/* Colour values in the range 0-255 */
typedef unsigned char byte;

/* Bitmap data for a single pixel */
typedef struct {
	byte red;
	byte green;
	byte blue;
} Colour;

typedef struct {
	char filename[16];
	int originalHeight;
	int originalWidth;
	int heightTop;
	int heightBottom;
} CutBitmapArgs;

typedef struct {
	int startLine;
	int endLine;
} Line;

typedef struct {
	int x;
	int y;
	int width;
	int height;
} Letter;

int **binaryPicture;
int save;
/* The maximum width and height of the image supported */
#define MAXW 1500
#define MAXH 1500

/* Function prototype declarations */
void LoadBMPFile(char *filename, int *width, int *height);
void SaveBMPFile(char *filename, int width, int height, int top, int bottom);

int GetPixelValue(int row, int col, int channel);
void SetPixelValue(int value, int row, int col, int channel);

void LoadColourArray(char *filename, Colour bitmap[MAXH][MAXW], int *width, int *height);
Line getNextLine(int width, int height);

void seperateLines(char* filename);
Letter getNextLetter(int startX, int top, int width, int height,int *space);
