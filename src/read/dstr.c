#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dstr.h"

dstring dstr_init(void){
	dstring temp;

	temp = malloc(1);
	temp[0] = '\0';

	return temp;
}

void dstr_addc(dstring *dstr, char c){
	UNINT len = strlen(TOCHAR *dstr) + 3;

	dstring temp;
	temp = malloc(len);

	memset(temp, '\0', len);
	strcpy(TOCHAR temp, TOCHAR *dstr);
	temp[len - 3] = c;

	free(*dstr);
	*dstr = temp;
}

void dstr_fputs(dstring *dstr){
	fputs(TOCHAR *dstr, stdout);
	fputc('\n', stdout);

	dstr_end(*(&dstr));
	*dstr = dstr_init();
}

void dstr_end(dstring *dstr){
	free(*dstr);
	*dstr = NULL;
}
