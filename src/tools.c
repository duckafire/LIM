#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

bool tools_strcmp2(char *str, char *v0, char *v1){
	if(str == NULL) return 0;
	return (strcmp(str, v0) == 0 || strcmp(str, v1) == 0);
}

FILE* tools_copyFile(FILE *src, char *destName){
	char c = 0;
	FILE *dest;
	const bool cloneBuf = (destName == NULL);

	// use `destName == NULL` for
	// temporary buffers
	if(cloneBuf)
		dest = tmpfile();
	else
		dest = fopen(destName, "w");
	
	fseek(src, 0, SEEK_SET);

	while(fread(&c, sizeof(char), 1, src) > 0 && c != EOF){
		if(cloneBuf)
			fwrite(&c, sizeof(c), 1, dest);
		else
			fprintf(dest, "%c", c);
	}

	fseek(src,  0, SEEK_SET);
	fseek(dest, 0, SEEK_SET);
	return dest;
}
