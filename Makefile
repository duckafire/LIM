CC = @gcc

FILES   = main arg tools write
SFILES  = $(addprefix ./src/, $(FILES))
SFILESC = $(addsuffix .c, $(SFILES))

PROGRAM = ./bin/lim.exe

all:	$(SFILESC)
	@$(CC) $(SFILESC) -o $(PROGRAM)
