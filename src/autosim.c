#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sndfile.h>
#include "assert.h"

#include <math.h>
#include <complex.h>
#include <fftw3.h>

#include "../lib/mfcc.h"

#define	FRAME_SIZE 8192
#define HOP_SIZE 2048


typedef struct mfcc {
  double data[MFCC_SIZE];
} mfcc;



static void print_usage (char *progname) {
  printf ("\nUsage : %s <input wav file1> <input wav file2> \n", progname) ;
	puts ("\n") ;
}

static void
fill_buffer(double *buffer, double *new_buffer)
{
  int i;
  double tmp[FRAME_SIZE-HOP_SIZE];

  /* save */
  for (i=0;i<FRAME_SIZE-HOP_SIZE;i++)
    tmp[i] = buffer[i+HOP_SIZE];

  /* save offset */
  for (i=0;i<(FRAME_SIZE-HOP_SIZE);i++)
    {
      buffer[i] = tmp[i];
    }

  for (i=0;i<HOP_SIZE;i++)
    {
      buffer[FRAME_SIZE-HOP_SIZE+i] = new_buffer[i];
    }
}

static int read_n_samples (SNDFILE * infile, double * buffer, int channels, int n)
{
  if (channels == 1) {
      /* MONO */
      int readcount ;
      readcount = sf_readf_double (infile, buffer, n);

      return readcount==n;

  } else if (channels == 2) {
      /* STEREO */
      double buf [2 * n] ;
      int readcount, k ;
      readcount = sf_readf_double (infile, buf, n);

      for (k = 0 ; k < readcount ; k++)
	       buffer[k] = (buf [k * 2]+buf [k * 2+1])/2.0 ;
      return readcount==n;

    } else {
      /* FORMAT ERROR */
      printf ("ERREUR: nombre de canaux non-traitable.\n");
    }
  return 0;
}


static int read_samples (SNDFILE * infile, double * buffer, int channels)
{
  return read_n_samples (infile, buffer, channels, HOP_SIZE);
}


float cosine_distance(mfcc c1, mfcc c2) {
	float product=0;
	float root1=0;
	float root2=0;
	int i;

	for(i=0;i<MFCC_SIZE;i++){
	  product += c1.data[i]*c2.data[i];
	  root1 += powf(c1.data[i],2);
	  root2 += powf(c2.data[i],2);
	}
	root1 = sqrtf(root1);
	root2 = sqrtf(root2);
	return product/(root1*root2);
}


float max(float a, float b) {
  if (a >= b) return a;
  else return b;
}


mfcc* fill_sequence(char* infilename, int *nb_frames) {
  SNDFILE	 	*infile = NULL ;
  SF_INFO	 	sfinfo ;

  if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL) {
      printf ("ERREUR: impossible d'ouvrir le fichier %s.\n", infilename) ;
      puts (sf_strerror (NULL)) ;
      return NULL ;
  }

  /* Read WAV and Compute MFCCs */
  *nb_frames = 0;
  double new_buffer[HOP_SIZE];
  double buffer[FRAME_SIZE];

  int i;
  for (i=0;i<(FRAME_SIZE/HOP_SIZE-1);i++) {
      if (read_samples (infile, new_buffer, sfinfo.channels)==1)
	      fill_buffer(buffer, new_buffer);
      else	{
    	  printf("ERREUR: pas assez d'échantillons fournis.\n");
    	  return NULL;
	    }
  }

  printf("# Taille : %d échantillons \n", (int)sfinfo.frames);
  int seq_size = (int)ceil((double)sfinfo.frames/HOP_SIZE);
  printf("# Nombre de symboles : %d \n", seq_size);

  mfcc* sequence1 = malloc(sizeof(struct mfcc)*seq_size);
  assert(sequence1);

  const int mfcc_size = MFCC_SIZE;
  double *m = malloc(sizeof(double)*mfcc_size);
  for (i = 0; i < mfcc_size; i++)
    m[i] = 0;

  mfcc_ctrl* mfcc_c=mfcc_init(FRAME_SIZE, MFCC_SIZE, 40, sfinfo.samplerate);

  while (read_samples (infile, new_buffer, sfinfo.channels)==1)
    {
      /* Process Samples */
      //printf("Processing frame %d\n",nb_frames);

      /* hop size */
      fill_buffer(buffer, new_buffer);

      /* Process */
      mfcc_compute(mfcc_c, buffer, FRAME_SIZE, m);

      for (i = 0; i < mfcc_size; i++)
	{
	  sequence1[(*nb_frames)].data[i] = m[i];
	}

      (*nb_frames)++;
    }

  sf_close (infile) ;
  mfcc_exit(mfcc_c);

  return sequence1;
}


int main (int argc, char * argv [])
{
  char 		*progname, *infilename1, *infilename2;
  // int i, j;

  progname = strrchr (argv [0], '/') ;
  progname = progname ? progname + 1 : argv [0] ;

  if (argc != 3)
    {	print_usage (progname) ;
      return 1 ;
    }

  infilename1 = argv [1] ;
  infilename2 = argv [2] ;

  int nb_frames1 = 0;
  mfcc * sequence1 = fill_sequence(infilename1, &nb_frames1);
  int nb_frames2 = 0;
  mfcc * sequence2 = fill_sequence(infilename2, &nb_frames2);

  if (sequence2 == NULL || sequence1 == NULL) {
      printf("ERREUR: calcul séquence.\n");
      return 1;
  }

  /* Comparaison */
  //TD SimilariteMFCC (cf code SimilariteMIDI)
  int matrix[nb_frames1+1][nb_frames2+1];

  for (int i = 0; i < nb_frames1+1; i++) {
    for (int j = 0; j < nb_frames2+1; j++) {
      matrix[i][j] = 0;
    }
  }

  int max1;
  float D = 0.0f;
  float match;
  for (int i = 1; i < nb_frames1+1; i++) {
    for (int j = 1; j < nb_frames2+1; j++) {
      match = cosine_distance(sequence1[i-1], sequence2[j-1]);
      max1 = max(matrix[i-1][j]-2, matrix[i][j-1]-2);
      matrix[i][j] = max(max1, matrix[i-1][j-1] + match);
      if (matrix[i][j] > D) D = matrix[i][j];
    }
  }

  printf("\n> Distance entre les deux extraits: %f\n", D);

  //Creation d'une image PPM de la matrice

  float distance[nb_frames1][nb_frames2];
  for (int i = 0; i < nb_frames1; i++) {
    for (int j = 0; j < nb_frames2; j++) {
      distance[i][j] = cosine_distance(sequence1[i], sequence2[j]);
    }
  }

  FILE *fp = fopen("sim_matrix.ppm", "w+");
  (void) fprintf(fp, "P2\n%d %d\n255\n", nb_frames1, nb_frames2);
  int color;
  for (int i = 0; i < nb_frames1; i++) {
    for (int j = 0; j < nb_frames2; j++) {
      color = 255.0*(1.0 + distance[i][j])/2.0;
      fprintf(fp, "%d ", color);
    }
    fprintf(fp, "\n");
  }
 (void) fclose(fp);



  if (!sequence1)
    free(sequence1);
  if (!sequence2)
    free(sequence2);

  return 0 ;
} /* main */
