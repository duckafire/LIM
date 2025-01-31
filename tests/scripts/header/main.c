#include <stdio.h>
#include <stdlib.h>
#include "../debug-common.h"
#include "../../../src/core/tools/queue.h"
#include "../../../src/core/tools/read-header-lim.h"
#include "../../../src/core/tools/lim-global-variables.h"

// (little) BUG:
// wrong "individual status" after use the
// arguments: 5, 6, 8, 12, 19, 20, 22, 26
// (it is not critical)


static FILE* write_and_print_header_lim(short code);
static void delete_header_lim(FILE *file);
static void set_lim_variables(void);
static void free_lim_variables(void);
static void print_status_and_address(short code);
static void print_buffer_content(void);
static void print_file(FILE *file, char *title);
static void print_queue(Queue *item);


struct Lim_Global_Variables lim;


#define N "\n@\n"
#define C0 "Hello world!"
#define C1 "a=0"
#define C2 "function_"
#define C3 "table_"
#define PRINT_TITLE(t) printf("[ %s ]\n", t)


int main(int argc, char *argv[]){
	CHECK_PROGRAM_ARG_QUANTITY

	const short code = (short)strtol(argv[1], NULL, 10);

	if(code < 0 || code > 27){
		INVALID_ARG_TO_MAIN("values between 0-27");
	}

	FILE *header_lim;
	header_lim = write_and_print_header_lim(code);

	set_lim_variables();
	print_status_and_address(code);
	print_buffer_content();

	free_lim_variables();
	delete_header_lim(header_lim);
	return 0;
}

static FILE* write_and_print_header_lim(short code){
	char c, d = EOF;
	FILE *file;
	const char *content[] = {
	/* 0000 $        */ NULL,
	/* 1000 -$       */      N,
	/* 1100 -@-$     */      N      N,
	/* 1110 -@-@-$   */      N      N      N,
	/* 1200 -@B$     */      N  C1,
	/* 1210 -@B@-$   */      N  C1  N,
	/* 1211 -@B@-@-$ */      N  C1  N      N,
	/* 1220 -@B@C$   */      N  C1  N  C2,
	/* 1221 -@B@C@-$ */      N  C1  N  C2  N,
	/* 1212 -@B@-@D$ */      N  C1  N      N  C3,
	/* 1222 -@B@C@D$ */      N  C1  N  C2  N  C3,
	/* 1120 -@-@C$   */      N      N  C2,
	/* 1121 -@-@C@-$ */      N      N  C2  N,
	/* 1122 -@-@C@D$ */      N      N  C2  N  C3,
	/* 1112 -@-@-@D$ */      N      N      N  C3,
	/* 2000 A$       */  C0  N,
	/* 2100 A@-$     */  C0  N      N,
	/* 2110 A@-@-$   */  C0  N      N      N,
	/* 2200 A@B$     */  C0  N  C1,
	/* 2210 A@B@-$   */  C0  N  C1  N,
	/* 2211 A@B@-@-$ */  C0  N  C1  N      N,
	/* 2220 A@B@C$   */  C0  N  C1  N  C2,
	/* 2221 A@B@C@-$ */  C0  N  C1  N  C2  N,
	/* 2212 A@B@-@D$ */  C0  N  C1  N      N  C3,
	/* 2222 A@B@C@D$ */  C0  N  C1  N  C2  N  C3,
	/* 2120 A@-@C$   */  C0  N      N  C2,
	/* 2121 A@-@C@-$ */  C0  N      N  C2  N,
	/* 2122 A@-@C@D$ */  C0  N      N  C2  N  C3,
	};


	file = fopen("header.lim", "w");

	if(content[code] != NULL)
		fputs(content[code], file);

	fclose(file);
	file = fopen("header.lim", "r");


	PRINT_TITLE("header.lim");

	while((c = fgetc(file)) != EOF){
		putchar(c);
		d = c;
	}

	if(d != '\n' && d != EOF)
		putchar('\n');

	PRINT_TITLE("header.lim");
	putchar('\n');
	fseek(file, 0, SEEK_SET);


	return file;
}

static void delete_header_lim(FILE *file){
	fclose(file);
	remove("header.lim");
}

static void set_lim_variables(void){
	lim.flags.header_file = 1;

	lim.header_partitions.top_header = NULL;
	lim.header_partitions.code_scope = NULL;
	lim.header_partitions.funct_list = NULL;
	lim.header_partitions.table_list = NULL;
}

static void free_lim_variables(void){
	if(lim.header_partitions.top_header != NULL)
		fclose(lim.header_partitions.top_header);

    if(lim.header_partitions.code_scope != NULL)
		fclose(lim.header_partitions.code_scope);

    qee_free_queue(lim.header_partitions.funct_list);
    qee_free_queue(lim.header_partitions.table_list);
}

static void print_status_and_address(short code){
	char *c_status = NULL;
	HF_OUT_STATUS status;
	const char code_expected[28][5] = { // 7 * 4 = 28
		"null", "1000", "1100", "1110", "1200", "1210", "1211",
    	"1220", "1221", "1212", "1222", "1120", "1121", "1122",
    	"1112", "2000", "2100", "2110", "2200", "2210", "2211",
    	"2220", "2221", "2212", "2222", "2120", "2121", "2122",
	};

	status = read_header_file(&c_status);
	
	printf("read_header_file status: %d\n\n", status);

	printf("Individual status: %s\n", c_status);
	printf("Status expected:   %s\n\n", code_expected[code]);
	
	puts("Buffers:");
	printf("%p\n", lim.header_partitions.top_header);
	printf("%p\n", lim.header_partitions.code_scope);
	printf("%p\n", lim.header_partitions.funct_list);
	printf("%p\n", lim.header_partitions.table_list);

	putchar('\n');
}

static void print_buffer_content(void){
	print_file(lim.header_partitions.top_header, "top_header");
	putchar('\n');

    print_file(lim.header_partitions.code_scope, "code_scope");
	putchar('\n');

	PRINT_TITLE("funct_list");
	print_queue(lim.header_partitions.funct_list);
	PRINT_TITLE("funct_list");
	putchar('\n');

	PRINT_TITLE("table_list");
	print_queue(lim.header_partitions.table_list);
	PRINT_TITLE("table_list");
	putchar('\n');
}

static void print_file(FILE *file, char *title){
	char c, d = EOF;

	PRINT_TITLE(title);

	if(file != NULL){
		fseek(file, 0, SEEK_SET);

		while((c = fgetc(file)) != EOF){
			putchar(c);
			d = c;
		}

		if(d != '\n' && d != EOF)
			putchar('\n');
	}

	PRINT_TITLE(title);
}

static void print_queue(Queue *item){
	if(item == NULL)
		return;

	print_queue(item->next);

	printf("%s\n", item->ident);
}
