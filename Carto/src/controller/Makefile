#
# SwarmBots - Makefile du package alphabot2 des sources.
# 
# @author Matthias Brun
# @author Fatoumata TRAORE: adaptation du makefile pour le PFE 2024
#

#
# Organisation des sources.
#

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

# Inclusion depuis le niveau du package.
#CCFLAGS += -I..
CCFLAGS += -std=c99 -I..

#
# Règles du Makefile.
#

.PHONY: all clean

# Compilation.
all: $(OBJ)

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@
	
# Nettoyage.
clean:
	@$(RM) $(OBJ) $(DEP)


-include $(DEP)
