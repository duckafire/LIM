#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../debug-common.h"
#include "../../../src/core/tools/lim-global-variables.h"
#include "../../../src/core/process-master.h"

FILE* write_and_print_source_file(short code);

#define SOURCE_LUA "./scripts/.tmp/source.lua"

int main(int argc, char *argv[]){
	CHECK_PROGRAM_ARG_QUANTITY

	short code = (short)strtol(argv[1], NULL, 10);

	lim_init_env();
	atexit(lim_free_env);

	if(code < 0 || code > 100){
		INVALID_ARG_TO_MAIN("values between 0-?");
	}

	lim.buffers.destine_file = tmpfile();
	lim.buffers.root.scope_func_pointer = tmpfile();
	lim.buffers.root.scope_func_address = tmpfile();
	lim.buffers.root.scope_var_tab = tmpfile();

	lim.files.source = write_and_print_source_file(code);
	puts("[ SOURCE.LIM ]");
	read_source_file();
	puts("\n[ SOURCE.LIM ]\n");

	return 0;
}

FILE* write_and_print_source_file(short code){
	char c;
	FILE *file;
	const char *content[] = {
"local foo0, stack1, cat2, var3 = 19",
"local vrum0, var1, cat2, car3 = 19",
"local java0, lang1, cat2, cat3, tbl4, script5, fish6, stack7, lang8, lang9 = car, 19, function()end",
"local java0 = 'Hello-world', false, function(A0,A1,A2)end",
"local foo0, foo1, var2, c3, car4, tab5, script6, cat7, car8",
"local foo0, vrum1, lang2, lang3, tab4, cat5, tab6, car7, script8",
"local lang0, stack1, stack2, c3, script4, var5, fish6 = 'hi'",
"local cat0, c1, tab2, car3",
"local tab0, foo1, stack2 = 12, 'lim', 19, false, 'lim'",
"local vrum0, tbl1, foo2, var3, var4, script5, tab6, cat7, java8 = 'hi'",
"local tbl0, tbl1, tab2, fish3, java4, script5, foo6, car7, tab8 = 'hi', 'hi', false, 'Hello-world'",
"local tab0, vrum1, vrum2, fish3, var4, fish5, tab6, cat7, tab8 = car, function(A0,A1,A2)end, 'lim', function()end, function(single)end, car, car",
"local stack0, vrum1 = foo, 'hi', function(A0,A1,A2)end",
"local tab0, car1, var2, cat3, foo4, vrum5 = function(single)end, false, car",
"local stack0, tab1, foo2, cat3, lang4, java5, java6, script7, tab8, c9 = foo",
"local c0 = 'hi', function(A0,A1,A2)end, 12, function(A0,A1,A2)end, 'Hello-world', 400",
"local lang0, stack1, tbl2, tab3, foo4, tbl5 = false, 'lim', false",
"local script0, tbl1, car2, vrum3, tbl4, car5, stack6 = function(single)end, 19, car, false, 400",
"local c0 = function(A0,A1,A2)end, 400, false, car, 'Hello-world', function(A0,A1,A2)end",
"local tbl0, java1, vrum2, java3, var4 = function(A0,A1,A2)end, 'lim', function()end, false",
"local java0, tbl1, c2, vrum3, fish4, tbl5, script6 = car, 400, false, 'lim', 400",
"local cat0, tbl1 = 400, foo",
"local tab0, vrum1, stack2, vrum3, car4, tbl5, car6, foo7",
"local fish0, script1, vrum2, tbl3, tbl4, lang5, stack6 = function(single)end, false, function(A0,A1,A2)end, 'Hello-world', foo, 19, function(A0,A1,A2)end, car",
"local car0, fish1, var2, var3, stack4, tab5, script6, tab7, cat8 = 'Hello-world', car",
"local tbl0, lang1, tab2, script3, cat4 = car, foo, 'hi', 400, false, 'hi'",
"local var0, stack1, foo2, c3, tab4 = car, function(single)end",
"local stack0, car1, java2, car3, stack4, stack5, stack6 = function()end, false, 'lim'",
"local var0, car1, cat2, lang3, vrum4, vrum5, java6, stack7 = 'Hello-world', 'lim', false, function(A0,A1,A2)end, 'Hello-world'",
"local c0, lang1, car2, cat3, tbl4 = function(A0,A1,A2)end, car, 'Hello-world', 19, function()end, function()end, false, 19, function(A0,A1,A2)end",
"local script0, foo1, cat2, tab3, c4, cat5, lang6, cat7 = function(A0,A1,A2)end, function(single)end, function(single)end, 12",
"local vrum0, vrum1 = car",
"local vrum0, vrum1, car2, cat3, tbl4, vrum5, stack6, cat7, stack8 = 'lim'",
"local cat0, vrum1 = car, function(A0,A1,A2)end, 12, 'Hello-world', function(A0,A1,A2)end, foo, function(A0,A1,A2)end, 'Hello-world'",
"local var0, var1, stack2, car3, tbl4, foo5, car6, foo7 = 400, 'lim', car, car",
"local lang0, car1, script2, tab3 = 'hi'",
"local stack0, tbl1, tab2, foo3, car4, cat5, fish6, tab7 = 'Hello-world', 400, false, 19, function()end, function(A0,A1,A2)end, car, car, 12",
"local lang0 = function(A0,A1,A2)end, 12, function(single)end, function()end, car, function(A0,A1,A2)end, false, foo, car",
"local cat0, stack1, fish2, cat3, vrum4, stack5 = 'lim', 'Hello-world', 400, function(A0,A1,A2)end, 'Hello-world', 'lim'",
"local cat0, stack1, foo2, c3, tbl4, java5, stack6, java7, var8 = 19, 'lim', function(single)end, function(A0,A1,A2)end, foo, false, 'lim', 'Hello-world', 'Hello-world'",
"local lang0, java1 = function()end",
"local tab0, c1, c2, foo3, tab4 = function()end, 19, car",
"local tbl0, tbl1, script2, vrum3, c4 = 12, function()end, foo, 'Hello-world', function(single)end, 400, 400, function(single)end",
"local lang0, cat1, java2, tab3, fish4, foo5, fish6, var7, lang8 = false, 'lim'",
"local var0 = function()end, 12, 'Hello-world', 19, 'hi', 19",
"local script0, java1, var2, vrum3, stack4, stack5, car6 = 12, function()end, function(single)end, foo, function(single)end, 12",
"local tbl0 = 'lim', 'lim', 'hi', 'hi', 400",
"local vrum0 = 12, 19, 12",
"local var0, tab1, stack2, tbl3, lang4 = 'hi'",
"local tab0, lang1, tbl2, lang3, java4 = 12, function()end, 'Hello-world', function()end, false, 'hi', 19",
"local car0, script1, tab2, car3, vrum4, script5, script6 = 'Hello-world'",
"local tab0, tbl1, java2, tab3, foo4, cat5, java6, vrum7",
"local tbl0, tab1, tbl2, tbl3",
"local vrum0, script1, tab2, car3, tbl4, script5, var6, tab7, vrum8, c9 = function()end, 'lim', false, false, 'Hello-world'",
"local vrum0 = function()end, function(A0,A1,A2)end, false, 'lim', 'lim', 19, 'hi'",
"local c0, c1, script2, cat3, script4, c5, fish6, tab7, tab8, car9 = 400",
"local c0, script1, script2, var3, fish4, tbl5, fish6 = 'lim', function()end, 12, 'Hello-world', 'Hello-world', 400",
"local vrum0, cat1, lang2, tbl3 = 'Hello-world', function(A0,A1,A2)end, function()end, function()end, 12, car, function(A0,A1,A2)end, function()end",
"local c0, java1, cat2, fish3 = function(single)end, 'lim', function(single)end, 'hi', false, foo, foo, 19",
"local vrum0, c1, var2, fish3 = 'hi', function(A0,A1,A2)end, 'hi', 400, 'hi'",
"local foo0, java1, stack2, tbl3, c4, c5, foo6, lang7, tab8, fish9 = 'hi', 'lim', car, car, 'hi', 'lim', 12",
"local foo0, stack1, tab2, vrum3, fish4, vrum5, car6, vrum7 = function()end",
"local lang0, lang1 = function()end, function(A0,A1,A2)end, 400, false, function(A0,A1,A2)end",
"local lang0, c1 = 'lim', foo, 'hi', function()end, 'Hello-world'",
"local c0, cat1, vrum2 = 'hi', 'hi', 400, function(A0,A1,A2)end, 'hi', car, 19, false, 'Hello-world'",
"local java0, car1, fish2, stack3, tab4, stack5 = function(A0,A1,A2)end, function(single)end, function()end, 400, 12, foo, 'hi', false, function()end",
"local cat0, car1 = car, function()end, false, foo",
"local fish0, fish1, stack2, tbl3, tbl4, java5, stack6, lang7, fish8, car9 = 'Hello-world', 400, 'Hello-world', 19, function(A0,A1,A2)end",
"local stack0, cat1, script2, var3, tbl4, tab5 = function(single)end",
"local lang0, lang1 = function()end, 'Hello-world', function(single)end, 19, false, 400, function()end",
"local script0, script1, vrum2, vrum3, foo4, stack5 = 400, function(single)end, car, 19, function(A0,A1,A2)end, foo",
"local vrum0, vrum1, script2",
"local var0 = function(A0,A1,A2)end, 12, false, 19, false",
"local var0, java1, stack2, cat3, tbl4 = 19, car",
"local tab0, foo1 = function()end, 19, car, 'Hello-world', function(single)end, 'Hello-world', function(single)end, foo, function(single)end",
"local car0, fish1, tbl2 = function(single)end, 'Hello-world', 400",
"local foo0, var1, lang2, var3 = 400, foo, 12, 19, 'Hello-world', function()end, car, car",
"local stack0 = function(single)end",
"local car0 = 'Hello-world', car, 19, 400, 12, 'Hello-world', 12",
"local var0, tab1, car2, cat3, java4, script5, cat6, tab7, script8, vrum9 = 12, 400, 19",
"local vrum0, car1, cat2, java3, fish4",
"local vrum0, tab1, c2, foo3, car4 = 'hi', 12, function(single)end, false, 400, foo, function()end, false",
"local tab0, var1, vrum2, cat3, cat4, lang5, foo6, vrum7 = 'lim', 400, 400, car, 'lim', function(single)end, false, 'lim', 12",
"local lang0, lang1, stack2 = 12",
"local vrum0, var1, cat2, c3, cat4 = function()end, 19, 'Hello-world', 19, function(A0,A1,A2)end, 'Hello-world', false",
"local stack0, c1 = function(A0,A1,A2)end, 400, 'lim'",
"local tbl0, tbl1, tbl2 = 12, function(single)end, foo, 'hi'",
"local tbl0, fish1, stack2, java3, tab4, fish5 = 12, car",
"local stack0, tbl1, var2, foo3, lang4, vrum5 = function(A0,A1,A2)end, function(A0,A1,A2)end, function(A0,A1,A2)end, car, function(single)end",
"local script0 = 19, 'lim', false, false, false, false, function(A0,A1,A2)end",
"local c0, tab1, java2, car3, var4, script5, tab6, java7, c8, script9 = 'lim', 12, car, 'hi', foo, false",
"local car0, tab1, tbl2, fish3, tbl4, c5, fish6 = 'Hello-world', function(A0,A1,A2)end, false, 12, foo, 'hi', car, 'Hello-world', 12",
"local c0, stack1, foo2, java3, stack4, tbl5, lang6, fish7, java8 = false, 19, foo",
"local c0, cat1, cat2, var3, vrum4, java5, c6, car7, foo8, cat9 = function(A0,A1,A2)end, false, function()end, 'Hello-world', function(single)end, foo, foo, function(single)end, function()end",
"local vrum0, lang1 = car, function(single)end, 'lim', function(single)end, car, 400, 'hi'",
"local cat0 = car, function(single)end, 19",
"local foo0, car1, var2 = function()end, 12, 'Hello-world', function(single)end, false, function(A0,A1,A2)end, foo",
"local script0, tab1, tab2, script3, c4, tbl5, fish6, script7, vrum8, fish9 = 12, 19, 12, 'Hello-world', 'Hello-world', 400, function()end",
"local lang0, fish1, tab2, tbl3, vrum4, c5 = car, 'Hello-world', 'hi', 'Hello-world', function(single)end, car, 19",
"local stack0, tbl1, tab2, lang3, tbl4, script5, stack6",
	};
	

	file = fopen(SOURCE_LUA, "w");
	if(errno == ENOENT){
		puts("Impossible find " SOURCE_LUA "\nGo to ./tests/\n");
		exit(1);
	}

	fputs(content[code], file);
	fclose(file);
	file = fopen(SOURCE_LUA, "r");


	puts("[ source.lua ]");

	while((c = fgetc(file)) != EOF)
		putchar(c);

	puts("\n[ source.lua ]\n");
	fseek(file, 0, SEEK_SET);


	return file;
}
