#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

FlagsVar flags = {false, false, true, 0};
LimVal lim = {NULL, NULL, NULL, {"courotine","debug","io","math","os","package","string","table", "utf8"}};

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
	if(cp_1_extractSourceContent())   return 0;
	if(cp_2_separateExtractContent()) return 0;
	if(cp_3_globalScopeTo_varFunc())         return 0;
	if(cp_4_localScopeTo_varFuncGParPar())    return 0;
	if(cp_5_organizeAndCompact())     return 0;
	cp_6_mergeContentAndPackLib();
	
	return 0;
}

static void cleanup(void){
	free(lim.destineFileName);
	ct_atexit();
}

