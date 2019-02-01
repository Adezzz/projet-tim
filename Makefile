#--------------------------------
# GENERAL VARIABLES
#--------------------------------

CC = gcc
CFLAGS = -O3 -I. -Wall -lm
SRC_DIR = ./src/
LIB_DIR = ./lib/

#--------------------------------
# LIBRARY SOURCES
#--------------------------------

LIB_SRC =  $(LIB_DIR)gnuplot_i.c
LIB_OBJ = $(LIB_SRC:.c=.o)
EXECS = wave ton rythme # rythme_beat rythme_tempo rythme_adapt

all: $(EXECS)

wave : $(LIB_OBJ) $(LIB_SRC) $(SRC_DIR)display_sound_wave.c
	$(CC) $(CFLAGS) $(LIB_OBJ) $(SRC_DIR)display_sound_wave.c -o wave -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3

ton :  $(LIB_DIR)midifile.c $(SRC_DIR)ton.c
	$(CC) $(LIB_DIR)midifile.c $(SRC_DIR)ton.c -o ton

# rythme_tempo : $(LIB_OBJ) $(LIB_SRC) $(SRC_DIR)rythme_tempo.c
# 	$(CC) $(CFLAGS) $(LIB_OBJ) $(SRC_DIR)rythme_tempo.c -o rythme_tempo -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3
#
# rythme_adapt : $(LIB_OBJ) $(LIB_SRC) $(SRC_DIR)rythme_adapt.c
# 	$(CC) $(CFLAGS) $(LIB_OBJ) $(SRC_DIR)rythme_adapt.c -o rythme_adapt -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3
#
# rythme_beat : $(LIB_OBJ) $(LIB_SRC) $(SRC_DIR)rythme_beatsynthesis.c
# 	$(CC) $(CFLAGS) $(LIB_OBJ) $(SRC_DIR)rythme_beatsynthesis.c -o rythme_beat -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3

rythme : $(LIB_OBJ) $(LIB_SRC) $(SRC_DIR)rythme.c
	$(CC) $(CFLAGS) $(LIB_OBJ) $(SRC_DIR)rythme.c -o rythme -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lm -lfftw3


clean :
	rm -f *.o *.wav *.raw
	rm $(EXECS)

cleanall :
	rm -f *.o *~ *.wav *.raw
	rm $(EXECS)
