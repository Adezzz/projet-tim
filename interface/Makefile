CPP=gcc    #Commande du compilateur
CFLAGS=-O3 #Option d'optimisation du programme
LDFLAGS=-lSDL -lSDL_mixer -lSDL_image #Linker
EXEC= window  #Nom du programme � modifier

all: ${EXEC}

${EXEC}: window.o
	${CPP} $(CFLAGS) -o window window.c ${LDFLAGS}

clean:
	rm -fr *.o

mrproper: clean
	rm -fr ${EXEC}
