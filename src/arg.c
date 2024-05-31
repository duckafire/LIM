#include <stdlib.h>
#include <string.h>

#include "defs.h"

int getFlags(int argc, char *argv[]){
    // none, null, -v, -h, -r
    //   -1,    0,   1, 2,  3

    if(argc == 1) return 0;

    if(argv[1][0] == '-'){
        if(strlen(argv[1]) == 2){
            char flags[3] = {'v', 'h', 'r'};
            for(int i = 0; i <= 3; i++) if(argv[1][1] == flags[i]) return i + 1;
        }
        perr("Invalid flag");
    }

    return -1;
}

void checkArgs(int argc, char *argv[], short replace, char **libName, char **libNoExt){
    // origin; libName (+1); quantity of arguments expected - all with "-r"
    short argID = 1;
    if(replace == 3) argID = 2; // without "-r"

    // get origin
    if(argc == argID) perr("<origin> not specified");

    if(strchr(argv[argID], '.') != NULL){
        if(strcmp(strchr(argv[argID], '.'), ".lua") != 0) perr("Extension not supported. Only \".lua\"");
    }else{
        perr("<origin> extension not specified (.lua)");
    }

    // libName: specified; not specified
    short add = 8;
    if(argc == argID + 1){
        argID--;
        add = 4;
    }

    const short len = strlen(argv[argID + 1]) + add;
    char *name, *noExt;
    name  = malloc(len);
    noExt = malloc(len);
    memset(name,  '\0', len);
    memset(noExt, '\0', len);

    strcpy(name, argv[argID + 1]);
    if(add == 4) name[len - 8] = '\0';
    strcpy(noExt, name);
    strcat(name, ".limfile");

    // it will be free in "main.c"
    *libName  = name;
    *libNoExt = noExt;
}

void argValid(char *argv){
    // file name
    if(isNum(argv[0])) perr("Invalid first caracter. Only letters");

    for(int i = 0; i < strlen(argv); i++){
        if(!firstChar(argv[i]) || !isNum(argv[i])) perr("Invalid character finded. Use only letters, numbers or underline.");
    }
}
