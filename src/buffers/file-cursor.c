#include <stdio.h>
#include "../heads.h"

static FILE *collect;

void collect_init(void){
	collect = tmpfile();
}

void collect_add(char c){
	fwrite(&c, sizeof(char), 1, collect);
}

void collect_end(void){
	fclose(copyFile(collect, "output.lim"));
	fclose(collect);
}
