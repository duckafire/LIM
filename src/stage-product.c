#include <stdbool.h>
#include "heads.h"

static FILE *toexport;

bool sp_extractSourceContent(short id, FILE *extrCttBuf){
	SP_BASE(id);

	t_copyAndExportFile(extrCttBuf);
	
	NONE_BUFFER_TO_FREE;
	return true;
}

bool sp_separateExtractContent(short id){
	SP_BASE(id);


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
		t_fcat(toexport, from->bufs[i]);

		fprintf(toexport, "\n\n");
	}

	// root function env.
	fprintf(toexport, "@@ FUNCTION ENVIRONMENTS\n@- [\"__fn_\" for anonymous functions]\n");

	for(i = from->head; i != NULL; i = i->next){
		fprintf(toexport, "@ FROM FUNCTION: %s\n- FUNCTIONS\n", i->name);
		t_fcat(toexport, i->bufs[ ADJ_LOCAL_TYPE( TYPE_LOCAL_FUNC_0 ) ]);
		
		fprintf(toexport, "\n- VARIABLES AND TABLES\n");
		t_fcat(toexport, i->bufs[ ADJ_LOCAL_TYPE( TYPE_LOCAL_VAR_1 ) ]);

		fprintf(toexport, "\n- ITSELF PARAMETERS\n");
		t_fcat(toexport, i->bufs[ ADJ_LOCAL_TYPE( TYPE_LOCAL_PSELF_2 ) ]);

		fprintf(toexport, "\n- FROM ALIGNED FUNCTIONS\n");
		t_fcat(toexport, i->bufs[ ADJ_LOCAL_TYPE( TYPE_LOCAL_PALIG_3 ) ]);
		
		fprintf(toexport, "\n\n");
	}

	info_verbose(VM_BREAK_FREE, "fromsrc", NULL);
	t_copyAndExportFile(toexport);
	fromsrc_end();
	return true;
}

bool sp_globalScopeTo_varFunc(short id){
	SP_BASE(id);


	toexport = tmpfile();

	fprintf(toexport, "@ IDENTIFIER SCOPE OF THE \"%s\"\n\n", lim.sourceFileName);
	t_fcat(toexport, scope_get(SCOPE_BASE));

	t_copyAndExportFile(toexport);
	fclose(toexport);


	info_verbose(VM_BREAK_FREE, "fromsrc", "scope", "pairs", NULL);
	fromsrc_end();
	scope_end();
	pairs_end();

	return true;
}

bool sp_localScopeTo_varFuncGParPar(short id){
	SP_BASE(id);

	// wip

	info_verbose(VM_BREAK_FREE, "fromsrc", "scope", "pairs", "local", NULL);
	fromsrc_end();
	scope_end();
	pairs_end();
	local_end();

	return true;
}

bool sp_organizeAndCompact(short id){
	SP_BASE(id);

	// wip

	return true;
}
