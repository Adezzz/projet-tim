#--------------------------------
# GENERAL VARIABLES
#--------------------------------

CC = gcc
CFLAGS = -O3 -I. -Wall -lm
SRC_DIR = src/
LIB_DIR = lib/
BIN_DIR = bin/

#--------------------------------
# LIBRARY SOURCES
#--------------------------------

LIB_SRC =  $(LIB_DIR)gnuplot_i.c
LIB_SIMI =  $(LIB_DIR)mfcc.c $(LIB_DIR)filterbank.c
LIB_OBJ = $(LIB_SRC:.c=.o)
EXECS = ton rythme similar autocor info main
all: $(EXECS)

main : $(SRC_DIR)main.c
	$(CC) $(CFLAGS) $^ -o $@

ton :  $(LIB_DIR)midifile.c $(SRC_DIR)ton.c
	$(CC) $^ -o $(BIN_DIR)$@

rythme : $(LIB_OBJ) $(LIB_SRC) $(SRC_DIR)rythme.c
	$(CC) $(CFLAGS) $(LIB_OBJ) $(SRC_DIR)rythme.c -o $(BIN_DIR)rythme -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3

similar : $(LIB_SIMI) $(SRC_DIR)sim_mfcc.c
	$(CC) $(CFLAGS)  $(LIB_SRC) $^ -o $(BIN_DIR)$@ -L./ -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3

autocor : $(LIB_SIMI) $(SRC_DIR)autosim.c
	$(CC) $(CFLAGS)  $(LIB_SRC) $^ -o $(BIN_DIR)$@ -L./ -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3

info : $(SRC_DIR)exec_soxi.c
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)$@

clean :
	rm -f *.o *.wav *.raw *.ppm
	rm main; cd $(BIN_DIR); rm *
