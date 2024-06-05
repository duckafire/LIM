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

int isNum(char c){
    return (c >= 48 && c <= 57);
}

int fCharOrNum(char c){
    return (firstChar(c) || isNum(c));
}

void saveState(FILE **origin, FILE **newFile, char *libName, char *libNoExt, FILE *buffer){
    // get file lenght
    fseek(*newFile, 0, SEEK_END);
    const long size = ftell(*newFile);
    fseek(*newFile, 0, SEEK_SET);

    // clear and open to update
    fclose(*origin);
    *origin = fopen(libName, "w");

    // start "do" block and decalre references
    if(buffer != NULL){
        char func[30], refe[5];

        // package init
        fprintf(*origin, "local %s={}\ndo local ", libNoExt);

        // variables
        fseek(buffer, 0, SEEK_SET);
        while(1){
            fseek(buffer, sizeof(func), SEEK_CUR);
            fread(refe, sizeof(refe), 1, buffer);
            if(feof(buffer)) break;
            fprintf(*origin, "%s,", refe);
        }
        fseek(*origin, -1, SEEK_CUR); // remove last ','
        fputc('=', *origin);

        // values
        fseek(buffer, 0, SEEK_SET);
        while(1){
            fread(func, sizeof(func), 1, buffer);
            if(feof(buffer)) break;
            fseek(buffer, sizeof(refe), SEEK_CUR);
            fprintf(*origin, "%s,", func);
        }
        fseek(*origin, -1, SEEK_CUR); // remove last ','
        fputc(' ', *origin);
    }
    // set and clear buffer
    char transfer[size + 5];
    memset(transfer, '\0', size);

    // copy file (binary to ASCII)
    fread(transfer, size, 1, *newFile);

    // remove trash in final of file
    if(buffer != NULL){
        short endFinded = 0;
        char signature[10] = "[#E#N#D#]";

        for(int i = sizeof(transfer); i > 0; i--){
            if(transfer[i] == ']'){

                if(transfer[i - 8] == '['){
                    endFinded = 1;

                    // check all character of the word finded
                    for(int j = 0; j < 9; j - j++){ // do not check '\0'{
                        if(signature[j] != transfer[i - 8 + j]){
                            endFinded = 0;
                            break; // only j1
                        }
                    }

                    // clear trash
                    if(endFinded == 1){
                        for(int j = i - 8; j <= sizeof(transfer); j++) transfer[j] = '\0';
                        endFinded = 2;
                        break;
                    }

                    // break i
                    if(endFinded == 2) break;
                }

            }
        }
    }

    fprintf(*origin, "%s", transfer);

    // close "do" block
    if(buffer != NULL) fprintf(*origin, " end\n--local reference=%s", libNoExt);

    // close and (re)open
    fclose(*origin);
    fclose(*newFile);
    *origin = fopen(libName, "r");
    *newFile = tmpfile();
}

int addSpace(FILE *origin){
    // ... @ [0] ...
    char last, next;

    if(ftell(origin) == 1){
        fseek(origin, 1, SEEK_CUR);
        // @ 0 [A] B...

        last = '#'; // any
    }else{
        fseek(origin, -2, SEEK_CUR);
        // ... [9] @ 0 @ 1 A B ...

        last = fgetc(origin);
        fseek(origin, 2, SEEK_CUR);
        // ... 9 @ 0 [@] 1 A B ...
    }

    while(1){
        next = fgetc(origin);
        // ... 9 @ 0 @ [1] A B ...

        if(next == '@'){
            fseek(origin, 1, SEEK_CUR);
            // ... 9 @ 0 @ 1 [A] B ...
            continue;
        }
        break;
    }
    // ... 9 @ 0 @ 1 A [B] ...

    fseek(origin, -2, SEEK_CUR);
    // ... 9 @ [0] @ 1 A B ...

    return ((firstChar(last) || isNum(last)) && firstChar(next));
}

int protectedWords(FILE *origin, FILE *newFile, char cc, short printID){
    char id;

    if(cc == '@'){
        if(!printID && addSpace(origin)) fputc(' ', newFile);

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

void refeBuffer(FILE *buffer, char *orgFunct, char *orgTable, char *refe){
    const short size = 30;
    const short sizf = 5;

    char func[size];
    memset(func, '\0', size); // clear and "fill"

    // get words
    if(orgTable != NULL){
        strcat(func, orgTable);
        strcat(func, ".");
    }
    strcat(func, orgFunct);

    // fill string of the reference
    for(int i = strlen(refe); i < sizf; i++) refe[i] = '\0';

    // check if its was already added
    char getted[size];
    fseek(buffer, 0, SEEK_SET);

    while(!feof(buffer)){
        memset(getted, '\0', size);
        if(fread(getted, sizeof(getted), 1, buffer) == 0) break;

        if(strcmp(func, getted) == 0) return;

        // jump reference
        fseek(buffer, sizf, SEEK_CUR);
    }
    // add
    fwrite(func, size, 1, buffer);
    fwrite(refe, sizf, 1, buffer);
}

int isLibFunc(char *name){
    short equal = 0;
    char prefix[5] = "LIB_";

    for(int i = 0; i < 4; i++){
        if(name[i] == prefix[i]) equal++;
    }

    return (equal == 4);
}
