#include <stdio.h>
#include "LibBMP.h"
#include "string.h"

int minimum(int a, int b, int c){
	if(a <= b && a <= c) return a;
	if(b <= c && b <= a) return b;
	if(c <= a && c <= b) return c;
}

int LevenshteinDistance(char* s, int len_s, char* t, int len_t)
{
  int cost = 0;
  if (len_s == 0) return len_t;
  if (len_t == 0) return len_s;
 
  if (s[len_s-1] == t[len_t-1]) cost = 0;
  else cost = 1;
 
  return minimum(LevenshteinDistance(s, len_s - 1, t, len_t    ) + 1,
                 LevenshteinDistance(s, len_s    , t, len_t - 1) + 1,
                 LevenshteinDistance(s, len_s - 1, t, len_t - 1) + cost);
}

char * compareWords(char *word){

	FILE *words;
        long elapsed_seconds;
	char filename[20] = "words2.txt";
	char line[200];
	int newLine = 0;
	int space = 0;

	int k = 0;

	if(word[0] == '\n') return word;

	while(k <= 200){
		if(word[k] == 0) break;
		if(word[k] == 32 ){
			 word[k] = 0;
			 space = 1;
			 break;
		}

		if(word[k] == '\n'){
			 word[k] = 0;
			 newLine = 1;
			 break;
		}
		k++;
	}

	for(k = 0;k < 200; k++)
		line[k] = 0;

	words = fopen(filename, "r");
	while(fgets(line,200,words)){

                sscanf (line, "%ld", &elapsed_seconds);
		k = 0;
		while(k <= 200){
			if(line[k] == 0) break;
			if(line[k] == 32 || line[k] == '\n') line[k] = 0;
			k++;
		}	
		if(strcasecmp(word,line) == 0){
			if(space){
				strcat(word," ");
        			fclose(words);
				return word;
			}

        		fclose(words);
			return word;
			
		}
		for(k = 0;k < 200; k++)
			line[k] = 0;
	
	}

	
	if(word[0] >= 65 && word[0] <= 90){
		if(space){
			strcat(word," ");
        		fclose(words);
			return word;
		}
        		fclose(words);
			return word;
	}



	words = fopen(filename, "r");

	while(fgets(line,200,words)){

                sscanf (line, "%ld", &elapsed_seconds);
		k = 0;
		while(k <= 200){
			if(line[k] == 0) break;
			if(line[k] == 32 || line[k] == '\n') line[k] = 0;
			k++;
		}
		if(strlen(word) == strlen(line) && !(line[0] >= 65 && line[0] <= 90))
			if(LevenshteinDistance(word,strlen(word),line,strlen(line)) == 1){
			if(space){
				strcat(line," ");
				fclose(words);
				return line;
			}
		}
		for(k = 0;k < 200; k++)
			line[k] = 0;
	
	}


	if(space){
		strcat(word," ");
		fclose(words);
		return word;
	}
	return word;
}

void correctText(){
	FILE *frsa,*final;
        frsa = fopen("output.txt", "r");
        final = fopen("final.txt", "w");
        long elapsed_seconds;
	char line[200];
	char word[200];

	int k = 0;
	for(k = 0; k < 200; k++){
		word[k] = 0;
		line[k] = 0;
	}


        while(fgets(line, 200, frsa) != NULL)
        {

                int i = 0;
		int j = 0;
                sscanf (line, "%ld", &elapsed_seconds);
                while(1)
		{
                        if(line[i] == 0) break;
				
			if(line[i] != 32){
				word[j++] = line[i];
			}

			if(line[i] == 32 || line[i] == '\n'){
				k = 0;
				if(line[i] == 32)
					word[j] = 32;

				while(k <= 200){
					if(word[k] == 32) break;
					if(k != 0 && word[k] >= 65 && word[k] <= 90)
						word[k] = word[k] + 32;
					k++;
				}

				
                		fprintf (final,"%s",compareWords(word));

				for(k = 0; k < 200; k++)
					word[k] = 0;

				j = 0;
			}
					

                        i++;
                }
		for(k = 0; k < 200; k++)
			line[k] = 0;
        }
        fclose(frsa);
        fclose(final);
}


int compareStrings(char* stringA, char* stringB){
	int i=0, same =1;
	while(stringA[i] != 0 && stringB[i] != 0 && same){
		if(stringA[i] != stringB[i])
			same = 0;		
		i++;
	}
	return same;
}


void main(int argc, char* argv[]){
	save = 0;
	if(argc > 2 && compareStrings(argv[2], "save")){
		save=1;
	}

	if(argc > 1)
		 extractText(argv[1]);
	else 
		puts("Give file"); 

	if(save != 1)
	correctText();
}
