# USE:	gcc,	make
CC = @gcc


# files
F_NAMES = main arg tools write
F_EXTEN = $(addprefix ./src/, $(F_NAMES))
F_FINAL = $(addsuffix .c, $(F_EXTEN))


# linux
L_ERRO = @echo Invalid target to Windows. Use only ´make´
L_PATH = ~/bin


# windows x linux
PRE_PROGRAM = ./bin/lim
OS = $(shell uname)

ifeq ($(OS), Winwods_NT)
	PROGRAM = $(addsuffix .exe, $(PRE_PROGRAM))
else
	PROGRAM = $(PRE_PROGRAM)
endif


# main
all:	$(PROGRAM)


# only linux
install:	$(PROGRAM)
	ifeq ($(OS), Linux)
		@cp $< $(L_PATH)
	else
		L_ERRO
	endif

uninstall:	$(L_PATH) $(PROGRAM)
	ifeq ($(OS), Linux)
		@rm $</$(PROGRAM)
	else
		L_ERRO
	endif

# compile to program
$(PROGRAM):	$(F_FINAL)
	$(CC) $^ -o $@ -I./src
