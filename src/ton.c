#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/midifile.h"

void read_midi(char * midifile);


void correlation(int *hist, int *res, double *base_hist_maj, double *base_hist_min){
  double sum_maj, sum_min;
  int gamme = 0;
  int note = 0;
  int max_sum = 0;


  for (int ton = 0; ton < 12; ton++) {
    sum_maj = 0;
    sum_min = 0;
    for (size_t i = 0; i < 12; i++) {
      sum_maj += ((double)hist[i])*base_hist_maj[(i + ton)%12];
      sum_min += ((double)hist[i])*base_hist_min[(i + ton)%12];
    }

    if ((sum_maj > sum_min) && (sum_maj > max_sum)) {
      max_sum = sum_maj;
      gamme = 0;
      note = ton;
    } else if ((sum_min > sum_maj) && (sum_min > max_sum)){
      max_sum = sum_min;
      gamme = 1;
      note = ton;
    }
  }

  res[0] = gamme; res[1] = 12 - note;
}


const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}


int main (int argc, char *argv[])
{

  //Check if file passed as argument
  if (argc < 2) {
    printf("usage: ./ton <midi_file>\n");
    return EXIT_SUCCESS;
  }
  //Check if file is MIDI
  if (strcmp("mid",get_filename_ext(argv[1])) != 0) {
    printf("Error: expected MIDI file, got %s file.\n",get_filename_ext(argv[1]));
    return EXIT_FAILURE;
  }

  read_midi(argv[1]);
}


void read_midi(char * midifile)
{
  int pitch; //duration, i, j, duration;
  int histogram[12] = {0};
  int nb_notes = 0;
  int res[2] = {0};

  // int profil[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

  double krumhansl_M[12] = {6.35,2.23,3.48,2.33,4.38,4.09,2.52,5.19,2.39,3.66,2.29,2.88};
  double krumhansl_m[12] = {6.33,2.68,3.52,5.38,2.6,3.53,2.54,4.75,3.98,2.69,3.34,3.17};

  double temperley_M[12] = {5,2,3.5,2,4.5,4,2,4.5,2,3.5,1.5,4};
  double temperley_m[12] = {5,2,3.5,4.5,2,4,2,4.5,3.5,2,1.5,4};

  MidiFileEvent_t event;
  MidiFile_t md = MidiFile_load(midifile);

  event = MidiFile_getFirstEvent(md);

  /* boucle principale */
  while(event)
    {
      if (MidiFileEvent_isNoteStartEvent(event))
	{
	  /* note start */

	  // Channel
	  int chan = MidiFileNoteStartEvent_getChannel(event);

	  // Attention channel 10 !!
	  if (chan != 10)
	    {
	      // Hauteur (Pitch)
	      pitch = MidiFileNoteStartEvent_getNote(event);
        nb_notes++;
	      // printf("Note jouee : %d, ", pitch);
        histogram[pitch%12]++;  //duree des notes non prise en consideration

	      // Durée (Duration)
	      // duration = MidiFileEvent_getTick(MidiFileNoteStartEvent_getNoteEndEvent(event))-MidiFileEvent_getTick(event);
	      // printf("duree %d\n", duration);

	      // Profil de notes à construire

	    }
	}

      event = MidiFileEvent_getNextEventInFile(event);
    }

  /* Profil de notes à afficher */
  printf("Note: ");
  for (int i = 0; i < 12; i++) {
    printf("%d ", i);
  }
   printf("\nOccu: ");
  for (size_t i = 0; i < 12; i++) {
    printf("%d ", histogram[i]);
  }

  /* Corrélation avec profils de référence */

  correlation(histogram, res, krumhansl_M, krumhansl_m);
  printf("\nCorrelation krumhansl: %d, %d\n", res[0], res[1]);
  correlation(histogram, res, temperley_M, temperley_m);
  printf("Correlation temperley : %d, %d\n", res[0], res[1]);

  /* Tonalité estimée à afficher */
}
