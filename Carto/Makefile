#
# SwarmBots - Makefile principal.
# 
# @author Matthias Brun
# @author Joshua MONTREUIL : adaptation du makefile pour ProSE pour l'equipe A2

# Repertoire d'installation des outils de developpement
# pour Raspberry Pi.
# EXEMPLE : RASPBERRY_TOOLS = /home/joshua/Documents/ProSe
RASPBERRY_TOOLS = /home/thomas/Tools/cross_compilation


# Compilateur croise (a adapter si besoin):
# EXEMPLE : export RASPBERRY_GCC = $(RASPBERRY_TOOLS)/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc

RASPBERRY_GCC = $(RASPBERRY_TOOLS)/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc

# Sysroot (a adapter si besoin):
# EXEMPLE1 : export RASPBERRY_SYSROOT = $(RASPBERRY_TOOLS)/tools/rootfs_bplus/
# EXEMPLE2 : export RASPBERRY_SYSROOT = /<path>/<to>/<raspberry>/<rootfs_bplus>/

RASPBERRY_SYSROOT = $(RASPBERRY_TOOLS)/tools/rootfs_bplus

# # Pour le pc de developpement.
# else
# # EXEMPLE : export CMOCKA = /home/joshua/Documents/Outils_Logiciels/CMocka/cmocka-1.1.5-x86_64/
# export CMOCKA = /home/joshua/Documents/Outils_Logiciel/lib/cmocka-x86_64
# endif

# Adresse de la Raspberry :
RASPBERRY_ADDRESS = <a>.<b>.<c>.<d>
# exemple : RASPBERRY_ADDRESS = 192.168.47.3


# Login/mot de passe de connexion à la Raspberry, à modifier si nécessaire :
RASPBERRY_LOGIN = pi
RASPBERRY_PASSWORD = swarmbots

# Terminal à utiliser pour la connexion ssh (nécessite sshpass et ssh)
TERM = gnome-terminal # gnome-terminal # xterm
TERMOPTIONS = -- # -- (pour gnome-terminal) # -e (pour xterm)


#
# Organisation des sources.
#

export SRCDIR = src

export BINDIR = bin

# Creation de doc Doxygen
export DOC = doc

SUBDIRS = $(SRCDIR) $(TESTDIR)

#
# Definitions des outils.
#

# Compilateur :

# Pour la cible Raspberry.
ifeq ($(TARGET), raspberry)
export CC = $(RASPBERRY_GCC)
export CCFLAGS += --sysroot=$(RASPBERRY_SYSROOT)
#export CCFLAGS += -I$(RASPBERRY_SYSROOT)/usr/local/include/ws2811
#export CCFLAGS += -I$(RASPBERRY_SYSROOT)/usr/include/gstreamer-1.0
#export CCFLAGS += -I$(RASPBERRY_SYSROOT)/usr/include/glib-2.0
export CCFLAGS += -I$(RASPBERRY_SYSROOT)/usr/lib/arm-linux-gnueabihf/glib-2.0/include/
export CCFLAGS += -I$(RASPBERRY_SYSROOT)/usr/include


# Pour le pc de developpement.
else 
export CC = gcc
endif

# sans debuggage :
export CCFLAGS += -O3 -DNDEBUG 
# avec debuggage : 
# export CCFLAGS += -Og -g -DDEBUG 

# dans tous les cas.
export CCFLAGS += -MMD -MP # gestion automatique des dependances
export CCFLAGS += -D_BSD_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE
export CCFLAGS += -std=c99 -Wall -pedantic

# Linker :

# Pour la cible Raspberry.
ifeq ($(TARGET), raspberry)
export LD = $(RASPBERRY_GCC)
export LDFLAGS += --sysroot=$(RASPBERRY_SYSROOT)
#export LDFLAGS += $(RASPBERRY_SYSROOT)/usr/local/lib/libws2811.a
#export LDFLAGS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0
export LDFLAGS += -lm -lrt -pthread -lwiringPi
endif

# Outils de documentation:
export DOXYGEN = doxygen
export DOXYFILE = Doxyfile
export DOXYGENFLAGS =

#
# Definition des binaires a generer.
#

# Pour la cible Raspberry.
ifeq ($(TARGET), raspberry)
export PROG_NAME = swarm_bots_raspberry.elf


# Pour le pc de developpement.
else 
export PROG_NAME = swarm_bots.elf

endif

export PROG = $(BINDIR)/$(PROG_NAME)


#
# Règles du Makefile.
#

.PHONY: all clean documentation $(SUBDIRS)

# Compilation.
all: $(SUBDIRS)

# Nettoyage.
clean: $(SUBDIRS)
	@rm -f $(PROG) $(TEST) core* $(BINDIR)/*
	@rm -rf $(DOC)/*

# Sous-répertoires.
$(SUBDIRS):
	$(MAKE) $(MAKECMDGOALS) -C $@

# Génération de la documentation.
documentation:
	$(DOXYGEN) $(DOXYGENFLAGS) $(DOXYGENCONF)
