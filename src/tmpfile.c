#include <stdio.h>

int main(int argc, char *argv[]){
    FILE *file;
    char msg[13];
    file = tmpfile();

    fwrite("Hello World!", sizeof(char), 13, file);
    fseek(file, 0, SEEK_SET);
    fread(msg, 1, sizeof(msg), file);

    printf("%s\n", msg);
    fclose(file);
    getchar(); // pause
    return 0;
}
