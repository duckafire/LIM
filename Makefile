CC = @gcc

FILES   = main arg
SFILES  = $(addprefix ./src/, $(FILES))
SFILESC = $(addsuffix .c, $(SFILES))

PROGRAM = ./bin/tin.exe

all:	$(SFILESC)
	@$(CC) $(SFILESC) -o $(PROGRAM)
