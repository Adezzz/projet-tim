#--------------------------------
# GENERAL VARIABLES
#--------------------------------

CC = gcc
CFLAGS = -O3 -I. -Wall -lm
SRC_DIR = src/
LIB_DIR = lib/

#--------------------------------
# LIBRARY SOURCES
#--------------------------------

LIB_SRC =  $(LIB_DIR)gnuplot_i.c
LIB_SIMI =  $(LIB_DIR)mfcc.c $(LIB_DIR)filterbank.c
LIB_OBJ = $(LIB_SRC:.c=.o)
EXECS = wave ton rythme similar main
all: $(EXECS)

main : $(SRC_DIR)main.c
	$(CC) $(CFLAGS) $(SRC_DIR)main.c -o main

wave : $(LIB_OBJ) $(LIB_SRC) $(SRC_DIR)display_sound_wave.c
	$(CC) $(CFLAGS) $(LIB_OBJ) $(SRC_DIR)display_sound_wave.c -o wave -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3

ton :  $(LIB_DIR)midifile.c $(SRC_DIR)ton.c
	$(CC) $(LIB_DIR)midifile.c $(SRC_DIR)ton.c -o ton

rythme : $(LIB_OBJ) $(LIB_SRC) $(SRC_DIR)rythme.c
	$(CC) $(CFLAGS) $(LIB_OBJ) $(SRC_DIR)rythme.c -o rythme -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3

similar : $(LIB_SIMI) $(SRC_DIR)sim_mfcc.c
	$(CC) $(CFLAGS) $(LIB_SIMI) $(LIB_SRC) $(SRC_DIR)sim_mfcc.c -o similar -L./ -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3

clean :
	rm -f *.o *.wav *.raw *.ppm
	rm $(EXECS)
