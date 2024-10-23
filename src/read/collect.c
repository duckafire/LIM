#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "head.h"
#include "../head.h"

static FILE *buffer;

void collect_init(void){
	buffer = tmpfile();
}

void collect_add(char c){
	fwrite(&c, sizeof(char), 1, buffer);
}

void collect_end(void){
	fclose(copyFile(buffer, "output.lim"));
	fclose(buffer);
}
