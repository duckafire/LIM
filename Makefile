CC?  =
B32? = false

FILES_NAMES = main arg stages tools-general tools-to-stages
FILES_PATH  = $(addprefix ./src/, $(FILES_NAMES))
FILES_FULL  = $(addsuffix .c,     $(FILES_PATH) )

FLAG_B32 =
NAME_B32 = ""

ifeq ($(B32), true)
	FLAG_B32 = -m32
	NAME_B32 = "-32"
endif

# e.g.:   gcc   [-m32]      <files.c>     -o ./bin/lim[-32][.exe]
COMPILE = $(CC) $(FLAG_B32) $(FILES_FULL) -o ./bin/lim$(NAME_B32)

all:
	@echo "+-------------------------------+"
	@echo "| Specifie a:                  |"
	@echo "| [target] linux | windows     |"
	@echo "| [compiler, e.g.] CC=gcc      |"
	@echo "| [if it will be x32] B32=true |"
	@echo "| Example: make linux CC=gcc   |"
	@echo "+------------------------------+"

windows:	$(FILES_FULL)
	@$(COMPILE).exe

linux:	$(FILES_FULL)
	@$(COMPILE)
