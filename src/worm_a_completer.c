#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <complex.h>
#include "gnuplot_i.h"

#define SAMPLING_RATE 44100.0
#define CHANNELS_NUMBER 1
#define N 512
#define NB_MAX_FRAME 20000
#define SEUIL_ENERGY 0.001
#define real double

typedef real frame[N];

static char *SOUND_FILE_NAME_READ = "tmp-in.raw";
static real cache_in[N/2];
static gnuplot_ctrl *h;

FILE *
sound_file_open_read (char *sound_file_name)
{
  char cmd[256];
  snprintf (cmd, 256,
	    "sox %s -c %d -r %d -b 16 -e signed-integer %s",
	    sound_file_name,
	    CHANNELS_NUMBER, (int)SAMPLING_RATE, SOUND_FILE_NAME_READ);
  system (cmd);
  return fopen (SOUND_FILE_NAME_READ, "rb");
}

void
sound_file_close_read (FILE *fp)
{
  fclose (fp);
  unlink (SOUND_FILE_NAME_READ);
}

int
sound_file_read (FILE *fp, frame s)
{
  int result, i;
  short tmp[N];

  result = fread (tmp, sizeof(short), N, fp);
  for (i=0; i<N; i++) s[i] = tmp[i] / 32768.0;

  return (result == N);
}

double energy(frame s)
{
  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += s[i]*s[i];
  }
  return sqrt(sum/(double)N);
}

int
main (int argc, char *argv[])
{
  FILE *input;
  frame s;

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <soundfile>\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  h = gnuplot_init();
  gnuplot_setstyle(h, "linespoints");

  input = sound_file_open_read (argv[1]);

  while (sound_file_read (input, s))
    {
	  /* a completer */
    }

  	  /* a completer */

  exit (EXIT_SUCCESS);
}
