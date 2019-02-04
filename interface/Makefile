CPP=gcc    #Commande du compilateur
CFLAGS=-O3 #Option d'optimisation du programme
LDFLAGS=-lSDL -lSDL_mixer #Linker
EXEC= window  #Nom du programme � modifier

all: ${EXEC}

${EXEC}: window.o
	${CPP} $(CFLAGS) -o window window.o ${LDFLAGS}

${EXEC}.o: window.c
	${CPP} $(CFLAGS) -o window.o -c window.c


clean:
	rm -fr *.o

mrproper: clean
	rm -fr ${EXEC}
