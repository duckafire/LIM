#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

int main(int argc, char *argv[]){
	// 1: tin --help
	// 2: tin <file>.lua <newFileName>
	// 3: tin <file>.lua <newFileName> --repl
	
	float id = checkArgs(argc, argv);
	
	if(id == 1.0){
		system("echo # ==================================================");
		system("echo # [ tin - v0.1.0 - https://github.com/duckafire/TIN");
		system("echo # ");
		system("echo # It is a small terminal program, created to compact");
		system("echo # files (libraries) of the TinyLibrary.");
		system("echo # ");
		system("echo # [!] Comands");
		system("echo # ");
		system("echo # \"tin --help\"");
		system("echo # \"tin <origin>.lua <newFileName>\"");
		system("echo # \"tin <origin>.lua <newFileName> --repl\"");
		system("echo # ==================================================");
		return 0;
	}
	system("echo [ WORK IN PROGRESS ]");
	return 0;
}
