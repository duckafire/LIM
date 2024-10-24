#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

bool g_verbose = false;
bool g_replace = false;

char *gp_nameOrg = NULL;
char *gp_nameDst = NULL;

FILE *gf_origin = NULL;

static bool dstUsingMalloc = false;

int main(int argc, char *argv[]){
	atexit(cleanup);

	// Check Flags
	cf_setArgValues(argc, argv);
	cf_single();
	cf_unexpected();
	cf_toCompaction(); // "destineName_1"
	cf_invalid();
	cf_originName();
	cf_destineName_2(&dstUsingMalloc);

	// Compaction Process
	cp_0_checkAndOpenFiles();
	cp_1_extractionFromOrigin();
	cp_x_tempFinish();

	return 0;
}

static void cleanup(void){
	if(dstUsingMalloc)
		free(gp_nameDst);

	if(gf_origin != NULL)
		fclose(gf_origin);
}

