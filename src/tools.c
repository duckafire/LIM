#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

bool strcmp2(char *str, char *v0, char *v1){
	if(str == NULL) return 0;
	return (strcmp(str, v0) == 0 || strcmp(str, v1) == 0);
}

FILE* copyFile(FILE *org, char *dstName){
	char c = 0;
	FILE *dst;
	const bool cloneBuf = (dstName == NULL);

	if(cloneBuf)
		dst = tmpfile();
	else
		dst = fopen(dstName, "w");
	
	fseek(org, 0, SEEK_SET);

	while(fread(&c, sizeof(char), 1, org) > 0 && c != EOF){
		if(cloneBuf)
			fwrite(&c, sizeof(c), 1, dst);
		else
			fprintf(dst, "%c", c);
	}

	return dst;
}
