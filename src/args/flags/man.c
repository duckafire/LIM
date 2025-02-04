#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "man.h"

bool flag_cmp(char *arg, char *f, char *flag){
	return (strcmp(arg, f) == 0 || strcmp(arg, flag) == 0);
}

void message(char n, ...){
	va_list parag;
	va_start(parag, n);

	char *line;

	while((line = va_arg(parag, char*)) != NULL)
		printf("%s\n", line);

	putchar('\n');
	va_end(parag);
	exit(0);
}
