#include <stdbool.h>
#include "heads.h"

static FILE *toexport;

bool stageProduct_extractionFromOrigin(short id, FILE *extrCttBuf){
	if(flags.untilStage != id)
		return false;

	info_verbose(VM_BREAK, 1);

	t_copyAndExportFile(extrCttBuf);
	
	return true;
}

bool stageProduct_separateExtractedContent(short id){
	if(flags.untilStage != id)
		return false;

	info_verbose(VM_BREAK, 2);


	const char* const titles[] = {
		"LIBRARY FUNCTIONS",
		"LIBRARY VARIABLES AND TABLES",
		"GLOBAL FUNCTIONS",
		"GLOBAL VARIABLES AND TABLES",
		"USE OR CALL",
		"CONSTANTS",
		"TABLES AND FUNCTIONS FROM Lua",
		"TABLES AND FUNCTIONS FROM \"header.lim\"",
	};

	GlobalEnv *from;
	FuncEnv *i;


	from = fromsrc_get();
	toexport = tmpfile();
	fromsrc_fseekSetAll();

	fprintf(toexport, "@@ ROOT ENVIRONMENTS\n");

	// root env. (jump "order file")
	for(short i = 1; i < FROMSRC_TOTAL_BUF; i++){
		fprintf(toexport, "@ %s\n", titles[i - 1]);
		t_fcat(from->bufs[i], toexport);

		fprintf(toexport, "\n\n");
	}

	// root function env.
	fprintf(toexport, "@@ FUNCTION ENVIRONMENTS\n@- [\"__fn_\" for anonymous functions]\n");

	for(i = from->head; i != NULL; i = i->next){
		fprintf(toexport, "@ FROM FUNCTION: %s\n- FUNCTIONS\n", i->name);
		t_fcat(i->bufs[0], toexport);
		
		fprintf(toexport, "\n- VARIABLES AND TABLES\n");
		t_fcat(i->bufs[1], toexport);
		
		fprintf(toexport, "\n\n");
	}

	t_copyAndExportFile(toexport);
	fromsrc_end();
	return true;
}

bool stageProduct_buildingGlobalScope(short id){
	if(flags.untilStage == id)
		return false;

	// wip

	return true;
}

bool stageProduct_organizeAndCompact(short id){}
