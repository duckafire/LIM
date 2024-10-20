#include <stdio.h>
#include <stdbool.h>

#include "../main.h"
#include "../messages/errors.h"
#include "start.h"
#include "input.h"
#include "dstr.h"

dstring gp_curWord;
FILE *gf_origin  = NULL;
FILE *gf_buffer  = NULL;

static char c;

void startProcess(void){
	gf_origin = fopen(gp_nameOrg, "r");
	if(gf_origin == NULL)
		nonExistentFile(gp_nameOrg);

	gf_buffer = NULL; // tmpfile();

	if(!g_replace && fopen(gp_nameDst, "r") != NULL)
		fileAlreadyExistent(gp_nameDst);

	gp_curWord = dstr_init();
	getContent();
}

static void getContent(void){
	bool fc = false; // it is the first character
	bool lf = false; // line feed
	bool fr = false; // the fisrt character (of the file) was writted

	while((c = fgetc(gf_origin)) != EOF){
		if((!fc && getSpace(c, fr, &lf)) || IS_BIN(c)) continue;

		if(getName(c, &fr, &fc)) continue;

		fc = lf = false;

		//if(getNum(c)) continue;
		//getSpecial(c);
	}

	dstr_fputs(&gp_curWord);
	dstr_end(&gp_curWord);
}
