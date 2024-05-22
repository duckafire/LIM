#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "defs.h"

void perr(char *msg){
	fprintf(stderr, "[LIM] %s.\n", msg);
	exit(1);
}

void pout(int qtt, ...){
	va_list msg;
	va_start(msg, qtt);

	for(int i = 0; i < qtt; i++) fprintf(stdout, "%s\n", va_arg(msg, char*));

	va_end(msg);
	exit(0);
}

void ckChar(char *word, char *blocked){
	for(int i = 0; i < strlen(word); i++){
		for(int j = 0; j < strlen(blocked); j++){
			if(word[i] <= 32 || word[i] == 127) perr("Controls character finded.");
			if(word[i] == blocked[j]) perr("Invalid character. Not use: / ? * | \" < > \\");
		}
	}
}

int ckFlag(char *word, char flags[][7]){
	for(int i = 0; i < FLAGS_QTT; i++){
		if(strcmp(word, flags[i]) == 0) return 1;
	}
	return 0;
}

void fileChar(char *_cc, char *_cf, FILE *origin){
	if(*_cf != EOF){
		*_cc = *_cf;
		*_cf = fgetc(origin);
	}else{
		*_cc = fgetc(origin);
	}
}

void clearSpace(FILE *file){
	char cc;
	while((cc = fgetc(file)) == ' ');
	fseek(file, -1, SEEK_CUR);
}

int firstChar(char c){
    // _, A-Z, a-z
    return (c == '_' || (c >= 65 && c <= 90) || (c >= 97 && c <= 122));
}

/*
int binScan(FILE *file, char *regex, char *word){
    int size = sizeof(word), reverse = 0;
    char temp[size];

    // get word in file
    fread(temp, 1, size, file);

    // finish with one break
    void loop(void){
        for(int i = 0; i < size; i++){
            
            for(int j = 0; j < strlen(regex); j++){
                if(regex[j] == '^' && !reverse){
                    reverse = 1;
                    continue;
                }

                // [123]
                if(!reverse){
                    for(int l = 0; l < strlen(regex); l++){
                        if(temp[i] != regex[l]) return;
                        if(regex[l] == '^') break;
                    }
                    continue;
                }

                // [^456]
                if(temp[i] == regex[i]) return;
            }

            // without "anomaly"
            word[size + 1] = temp[i];
        
        }
    }

    loop();
    word[size + 1] = '\0';
    return strlen(word);
}
*/
