#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "head.h"

static FILE *buffer;

void collect_init(void){
	buffer = tmpfile();
}

void collect_add(char c){
	fwrite(&c, sizeof(char), 1, buffer);
}

void collect_end(void){
	fseek(buffer, 0, SEEK_SET);

	char c = 0;
	FILE *output = fopen("output.lim", "w");

	while(fread(&c, sizeof(char), 1, buffer) > 0 && c != EOF)
		fprintf(output, "%c", c);

	fclose(output);
	fclose(buffer);
}
