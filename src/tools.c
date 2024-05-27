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

void saveState(FILE **origin, FILE **newFile, char *libName){
    // get file lenght
    fseek(*newFile, 0, SEEK_END);
    long size = ftell(*newFile);
    fseek(*newFile, 0, SEEK_SET);

    // clear and open to update
    fclose(*origin);
    *origin = fopen(libName, "w");

    // set and clear buffer
    char transfer[size];
    memset(transfer, '\0', size);

    // copy file (binary to ASCII)
    fread(transfer, size, 1, *newFile);
    fprintf(*origin, "%s", transfer);
    
    // close and (re)open 
    fclose(*origin);
    fclose(*newFile);
    *origin = fopen(libName, "r");
    *newFile = tmpfile();
}

int protectedWords(FILE *origin, FILE *newFile, char cc, short printID){
    char id;
    
    if(cc == '@'){
        id = fgetc(origin);

        if(printID) fprintf(newFile, "%c%c", cc, id);

        while(1){
            cc = fgetc(origin);

            if(cc == '@'){
                // check if it is the end
                if((cc = fgetc(origin)) == id){
                    if(printID) fprintf(newFile, "@%c", cc);
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

// referenceHead(refHead, NULL,     NULL, NULL, NULL);
// referenceHead(refHead, *newFile, ----, NULL, libName);
// referenceHead(refHead, *newFile, ----, ----, libName);
void referencesHead(FILE *buffer, FILE *newFile, char *orgFunct, char *orgTable, char *reference, char *libName){
    char *func[50], *refe[5];
    
    memset(func, '\0', 50);
    memset(refe, '\0',  5);

    fseek(buffer, 0, SEEK_SET);
    
    // add to buffer
    if(newFile == NULL){
        // get characters
        if(orgTable != NULL){
            strcpy(func, orgTable);
            strcat(func, ".");
        }
        strcat(func, orgFunc);

        // fill
        for(int i = strlen(func); i < sizeof(func); i++) func[i] = '\0';
        for(int i = strlen(refe); i < sizeof(refe); i++) refe[i] = '\0';

        // check if its was already added
        char getted[50];
        while(!feof(buffer)){
            memset(getted, '\0', 50);
            fread(getted, sizeof(get), 1, buffer);
            
            if(strcmp(func, getted) == 0) return;

            fseek(buffer, sizeof(refe), SEEK_CUR);
        }

        // add
        fseek(buffer, 0, SEEK_END);
        fwrite(func, sizeof(func), 1, buffer);
        fwrite(refe, sizeof(refe), 1, buffer);

        return;
    }
    
    // add to final file (library)
    fseek(newFile, 0, SEEK_SET);
    
    fprintf(newFile, "local %s={}\ndo local ", libName);

    void add(char *name){
        while(1){
            memset(name, '\0', sizeof(name));

            fread(name, sizeof(name), 1, buffer);

            fprintf(newFile, "%s", name);
            
            if(!feof(buffer)){
                fputs(', ', newFile);
            }else{
                break;
            }
        }
    }

    add(func);
    add(refe)

    fprintf(newFile, " end\n--local reference=%s", libName);
}
