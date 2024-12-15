#include <stdbool.h>
#include "heads.h"

static FILE *toexport;

bool sp_extractSourceContent(short id, FILE *extrCttBuf){
	SP_BASE(id);

	t_copyAndExportFile(extrCttBuf);
	fclose(extrCttBuf);
	
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

	EXPORT;


	info_verbose(VM_BREAK_FREE, "fromsrc", NULL);
	fromsrc_end();
	return true;
}

bool sp_globalScopeTo_varFunc(short id){
	SP_BASE(id);


	Queue *pairs;

	toexport = tmpfile();

	fprintf(toexport, "@ IDENTIFIER SCOPE OF THE \"%s\"\n\n", lim.sourceFileName);
	t_fcat(toexport, scope_get(SCOPE_BASE));

	fprintf(toexport, "\n\n@ VARIABLES, TABLES AND FUNCTIONS NICKNAME\n- FUNCTIONS FROM Lua and \"header.lim\"\n");
	for(short i = 0; i < 2; i++){
		for(pairs = pairs_get(i); pairs != NULL; pairs = pairs->next)
			fprintf(toexport, "%s\t%s\n", pairs->content[0], pairs->content[1]);

		if(i == 0)
			fprintf(toexport, "\n- \"GLOBAL PROVATE\" VARIABLES, TABLES AND FUNCTIONS\n");
	}

	EXPORT;


	info_verbose(VM_BREAK_FREE, "fromsrc", "scope", "pairs", NULL);
	fromsrc_end();
	scope_end();
	pairs_end();

	return true;
}

bool sp_localScopeTo_varFuncGParPar(short id){
	SP_BASE(id);


	Queue *pairs;
	FuncEnv *func;

	toexport = tmpfile();

	fprintf(toexport, "@@ SCOPES AND NICKNAME-IDENT. FROM \"ROOT FUNCTIONS\"\n\n");

	for(func = local_get(); func != NULL; func = func->next){
		fprintf(toexport, "@ NICKNAMES AND IDENTIFIERS FROM FUNCTION: %s\n", func->name);

		fprintf(toexport, "- SCOPE: \t");
		if(IS_VALID_SCOPE(func->scope))
			t_fcat(toexport, func->scope);

		fputc('\n', toexport);

		for(pairs = func->pairs; pairs != NULL; pairs = pairs->next)
			fprintf(toexport, "%s\t%s\n", pairs->content[0], pairs->content[1]);

		fputs("\n", toexport);
	}

	EXPORT;


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
