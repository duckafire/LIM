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

void saveState(FILE **origin, FILE **newFile){
    // get file lenght
    fseek(*newFile, 0, SEEK_END);
    long size = ftell(*newFile);
    fseek(*newFile, 0, SEEK_SET);

    // clear and open to update
    fclose(*origin);
    *origin = fopen(".limfile", "w");

    // set and clear buffer
    char transfer[size];
    memset(transfer, '\0', size);

    // copy file (binary to ASCII)
    fread(transfer, size, 1, *newFile);
    fprintf(*origin, "%s", transfer);
    
    // close and (re)open 
    fclose(*origin);
    fclose(*newFile);
    *origin = fopen(".limfile", "r");
    *newFile = tmpfile();
}

int protectedWords(FILE *origin, FILE *newFile, char cc, short printID){
    //fseek(origin, -1, SEEK_CUR);
    
    char id;//, last = fgetc(origin);
    
    if(cc == '@'){
        id = fgetc(origin);

        if(printID) fprintf(newFile, "%c%c", cc, id);// else if(firstChar(last)) fputc(' ', newFile);

        while(1){
            cc = fgetc(origin);

            if(cc == '@'){
                // check if it is the end
                if((cc = fgetc(origin)) == id){
                    //last = fgetc(origin); // *next
                    if(printID) fprintf(newFile, "@%c", cc); //else if(firstChar(last)) fputc(' ', newFile);
                    break;
                }
                continue;
            }
            if(cc == EOF) break;

            fputc(cc, newFile);
        }
        // if the conditions is true, they are protected words
        return 1;
    }

    return 0;
}

void wordsBuffer(FILE *buffer, char *word){
    // buffer to store searched words
    char store[50];

    rewind(buffer);

    // searsh word in buffer
    while(fread(store, 50, 1, buffer) != 0){
        // check if the word already was writed in buffer
        if(strcmp(word, store) == 0) return;
    }

    // fill
    strcpy(store, word);
    for(int i = strlen(store); i <= sizeof(store); i++) store[i] = '\0';

    // save word
    fwrite(store, sizeof(store), 1, buffer);
}
