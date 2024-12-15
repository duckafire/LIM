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
	fromsrc_fseekSetAll();
	EXP_BUF;

	TOEXP_PRINT("@@ ROOT ENVIRONMENTS\n");

	// root env. (jump "order file")
	for(short i = 1; i < FROMSRC_TOTAL_BUF; i++){
		TOEXP_PRINT("@ %s\n", titles[i - 1]);
		TOEXP_FCAT(from->bufs[i]);

		TOEXP_PRINT("\n\n");
	}

	// root function env.
	TOEXP_PRINT("@@ FUNCTION ENVIRONMENTS\n@- [\"__fn_\" for anonymous functions]\n");

	for(i = from->head; i != NULL; i = i->next){
		TOEXP_PRINT("@ FROM FUNCTION: %s\n- FUNCTIONS\n", i->name);
		TOEXP_FCAT(i->bufs[ ADJ_LOCAL_TYPE( TYPE_LOCAL_FUNC_0 ) ]);
		
		TOEXP_PRINT("\n- VARIABLES AND TABLES\n");
		TOEXP_FCAT(i->bufs[ ADJ_LOCAL_TYPE( TYPE_LOCAL_VAR_1 ) ]);

		TOEXP_PRINT("\n- ITSELF PARAMETERS\n");
		TOEXP_FCAT(i->bufs[ ADJ_LOCAL_TYPE( TYPE_LOCAL_PSELF_2 ) ]);

		TOEXP_PRINT("\n- FROM ALIGNED FUNCTIONS\n");
		TOEXP_FCAT(i->bufs[ ADJ_LOCAL_TYPE( TYPE_LOCAL_PALIG_3 ) ]);
		
		TOEXP_PRINT("\n\n");
	}

	EXP_NOW;


	info_verbose(VM_BREAK_FREE, "fromsrc", NULL);
	fromsrc_end();
	return true;
}

bool sp_globalScopeTo_varFunc(short id){
	SP_BASE(id);


	Queue *pairs;
	EXP_BUF;

	TOEXP_PRINT("@ IDENTIFIER SCOPE OF THE \"%s\"\n\n", lim.sourceFileName);
	TOEXP_FCAT(scope_get(SCOPE_BASE));

	TOEXP_PRINT("\n\n@ VARIABLES, TABLES AND FUNCTIONS NICKNAME\n- FUNCTIONS FROM Lua and \"header.lim\"\n");
	for(short i = 0; i < 2; i++){
		for(pairs = pairs_get(i); pairs != NULL; pairs = pairs->next)
			TOEXP_PRINT("%s\t%s\n", pairs->content[0], pairs->content[1]);

		if(i == 0)
			TOEXP_PRINT("\n- \"GLOBAL PROVATE\" VARIABLES, TABLES AND FUNCTIONS\n");
	}

	EXP_NOW;


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
	EXP_BUF;

	TOEXP_PRINT("@@ SCOPES AND NICKNAME-IDENT. FROM \"ROOT FUNCTIONS\"\n\n");

	for(func = local_get(); func != NULL; func = func->next){
		TOEXP_PRINT("@ NICKNAMES AND IDENTIFIERS FROM FUNCTION: %s\n", func->name);

		TOEXP_PRINT("- SCOPE: \t");
		if(IS_VALID_SCOPE(func->scope))
			TOEXP_FCAT(func->scope);

		TOEXP_PRINT("\n");

		for(pairs = func->pairs; pairs != NULL; pairs = pairs->next)
			TOEXP_PRINT("%s\t%s\n", pairs->content[0], pairs->content[1]);

		TOEXP_PRINT("\n\n");
	}

	EXP_NOW;


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
