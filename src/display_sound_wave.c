#include "../lib/gnuplot_i.h"

#define SAMPLING_RATE 44100.0
#define CHANNELS_NUMBER 1
#define N 1024

char *RAW_FILE = "tmp-in.raw";
int r;

FILE *sound_file_open_read (char *sound_file_name) {
  char cmd[256];
  snprintf (cmd, 256,
	    "sox %s -c %d -r %d %s",
	    sound_file_name,
	    CHANNELS_NUMBER, (int)SAMPLING_RATE, RAW_FILE);
  if( (r = system (cmd)) == -1) {
    perror("Error while runnning sox.\n");
    exit(1);
  }

  return fopen (RAW_FILE, "rb");
}


void sound_file_close_read (FILE *fp) {

  fclose (fp);
}


int sound_file_read (FILE *fp, double *buf) {
  return fread(buf, sizeof(double), 1, fp);
}


int main (int argc, char *argv[]) {

  FILE *fp;
  double s[N];
  int pos = 0;

  //Check if file passed as argument
  if (argc < 2) {
    printf("usage: ./prog <wav_file>\n");
    return EXIT_SUCCESS;
  }

  //Load file in pointer
  fp = sound_file_open_read(argv[1]);
  if (fp == NULL) {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  } else {
    printf("File loaded\n");
  }

  //Display sound wave
  gnuplot_ctrl *h = gnuplot_init();
  gnuplot_setstyle(h, "lines");
  printf("%s\n", argv[1]);

  while(sound_file_read (fp, s) > 0 && pos < N) {

      gnuplot_cmd(h, "set yr [-1:1]");
      gnuplot_resetplot(h);

      gnuplot_plot_x(h,s,N,"Plot");
      pos++;
      sleep(N * 2 / SAMPLING_RATE);
    }

  sound_file_close_read (fp);
  exit (EXIT_SUCCESS);
}
