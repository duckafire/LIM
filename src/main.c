#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

bool g_verbose = false;
bool g_replace = false;
bool g_headfile = true;

char *gp_nameOrg = NULL;
char *gp_nameDst = NULL;

FILE *gf_origin = NULL;

int main(int argc, char *argv[]){
	atexit(cleanup);

	// Check Flags
	cf_setArgValues(argc, argv);
	cf_single();
	cf_unexpected();
	cf_toCompaction(); // "destineName_1"
	cf_invalid();
	cf_originName();
	cf_destineName_2();

	// Compaction Process
	cp_0_checkAndOpenFiles();
	cp_1_extractionFromOrigin();
	cp_2_separateExtractedContent();
	cp_3_buildingGlobalScope();
	cp_4_organizeAndCompact();
	cp_x_mergingContentAndPackingLibrary();

	return 0;
}

static void cleanup(void){
	free(gp_nameDst);

	if(gf_origin != NULL)
		fclose(gf_origin);

	buffers_atexit();
	ct_atexit();
	head_end();
}

