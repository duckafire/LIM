#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "global.h"

bool strcmp2(char *str, char *v0, char *v1){
	if(str == NULL) return 0;
	return (strcmp(str, v0) == 0 || strcmp(str, v1) == 0);
}

void pMessage(char n, ...){
	va_list parag;
	va_start(parag, n);

	char *cur;

	while((cur = va_arg(parag, char*)) != NULL)
		fprintf(stdout, "%s\n", cur);

	fputc('\n', stdout);
	va_end(parag);
	exit(0);
}
