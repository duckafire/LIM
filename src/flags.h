#ifndef lim_flags
#define lim_flags

// 7 + 2 + 1
#define BIG_FLAG 10

#define VERSION "-v","--version"
#define MANUAL  "-m","--manual"
#define HELP    "-h","--help"
#define VERBOSE "-V","--verbose"
#define NAME    "-n","--name"
#define REPLACE "-r","--replace"

void pParag(char **parag);

void welcome(void); // called without arguments
void version(void);
void manual(void);
void help(char *flag);
void verbose(void);

#endif

/*
void verbose(void){
	if(!g_verbose) return;

	static short id = -1;
	id++;

	if(id == 0){
		printf("lorem ipsum");
		return;
	}
	....
}
*/

