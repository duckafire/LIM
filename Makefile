# USE:	gcc,	make
CC = @gcc
EC = @echo - LIM was


# files
F_NAMES = main arg stages tools-general tools-to-stages
F_EXTEN = $(addprefix ./src/, $(F_NAMES))
F_FINAL = $(addsuffix .c, $(F_EXTEN))


# linux
L_ERRO = @echo Invalid target to Windows. Use only ´make´
L_PATH = /bin/lim


# windows x linux
OS = $(shell uname)

ifeq ($(OS), Winwods_NT)
	PRE_PROGRAM = $(addsuffix .exe, $(L_PATH))
	L_ADD = $(L_ERRO)
	L_SUB = $(L_ERRO)
else
	PRE_PROGRAM = $(L_PATH)
	L_ADD = @cp $< $(PRE_PROGRAM)
	L_SUB = @rm $^
endif


# executable
PROGRAM = $(addprefix ., $(PRE_PROGRAM))


# targets
all:	$(F_FINAL)
	$(CC) $^ -o $(PROGRAM) -I./src
	$(EC) compiled to ./bin

install:	$(PROGRAM)
	$(L_ADD)
	$(EC) added to /bin

uninstall:	$(PRE_PROGRAM)
	$(L_SUB)
	$(EC) removed to /bin

