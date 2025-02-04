#include <string.h>
#include <stdbool.h>
#include "cmp.h"

bool flag_cmp(char *arg, char *f, char *flag){
	return (strcmp(arg, f) == 0 || strcmp(arg, flag) == 0);
}
