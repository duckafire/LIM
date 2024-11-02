#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

static char c;

void cp_0_checkAndOpenFiles(void){
	info_verbose();

	gf_origin = fopen(gp_nameOrg, "r");
	if(gf_origin == NULL)
		nonExistentFile(gp_nameOrg);

	if(!g_verbose){
		FILE *dst;
		dst = fopen(gp_nameDst, "r");

		if(dst != NULL){
			fclose(dst);
			fileAlreadyExistent(gp_nameDst);
		}
	}

	collect_init();
}

void cp_1_extractionFromOrigin(void){
	info_verbose();

	// for not add line feed after dot
	// from float number
	bool isFloat = false;

	while(FGETC != EOF){
		// clear "null caracteres", without
		// `continue` cycle: They are:
		// // spaces
		// // tabulations
		// // line feed
		// // binaries
		if(!isgraph(c))
			while(!isgraph(FGETC) && c != EOF);

		// get identifiers to:
		// // variables
		// // tables
		// // tables keys
		// // functions
		if(getIdentifier(&c, true)){
			do{
				// the dot in first will protect
				// this key against the compaction
				// process
				if(c == '.')
					collect_add('\n');

				collect_add(c);

				// this parenthesis specify that
				// this identify belongs to function
				if(c == '(')
					break;

				FGETC;
			}while(getIdentifier(&c, false));
			
			collect_add('\n');
			continue;
		}

		// constant numbers:
		// integer
		// float
		if(isdigit(c)){
			do{
				collect_add(c);
				FGETC;

				if(c == ' ')
					c = clearSpaces();

				// when the first dot is finded, `isFloat`
				// becomes `true`, after the loop cycle
				// come back to here it will become `false`,
				// and it cannot become `true` again in this
				// loop
				if(c == '.' || isFloat)
					isFloat = !isFloat;

			}while(isdigit(c) || isFloat);

			isFloat = false;
			collect_add('\n');
			
			// if a second dot was finded
			if(c == '.'){
				collect_add('.');
				collect_add('\n');
			}

			continue;
		}

		// special characteres (:,.{-+)
		getSpecial(c);
	}
}

void cp_x_tempFinish(void){
	info_verbose();
	collect_end();
	info_verbose();
}
