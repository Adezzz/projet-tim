#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sndfile.h>
#include "assert.h"

#include <math.h>
#include <complex.h>
#include <fftw3.h>

#define	FRAME_SIZE 8192
#define HOP_SIZE 2048

#define CHROMA_SIZE 12

static fftw_plan plan = NULL;

typedef struct chroma {
  double data[CHROMA_SIZE];
}
  chroma;



static void
print_usage (char *progname)
{	printf ("\nUsage : %s <input wav file1> <input wav file2> \n", progname) ;
	puts ("\n"
		) ;

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

static int
read_n_samples (SNDFILE * infile, double * buffer, int channels, int n)
{

  if (channels == 1)
    {
      /* MONO */
      int readcount ;

      readcount = sf_readf_double (infile, buffer, n);

      return readcount==n;
    }
  else if (channels == 2)
    {
      /* STEREO */
      double buf [2 * n] ;
      int readcount, k ;
      readcount = sf_readf_double (infile, buf, n);
      for (k = 0 ; k < readcount ; k++)
	buffer[k] = (buf [k * 2]+buf [k * 2+1])/2.0 ;

      return readcount==n;
    }
  else
    {
      /* FORMAT ERROR */
      printf ("Channel format error.\n");
    }
  
  return 0;
} 

static int
read_samples (SNDFILE * infile, double * buffer, int channels)
{
  return read_n_samples (infile, buffer, channels, HOP_SIZE);
}

void
fft_init (complex in[FRAME_SIZE], complex spec[FRAME_SIZE])
{
  plan = fftw_plan_dft_1d (FRAME_SIZE, in, spec, FFTW_FORWARD, FFTW_ESTIMATE);
}

void
fft_exit (void)
{
  fftw_destroy_plan (plan);
}

void
fft_process (void)
{
  fftw_execute (plan);
}

float cosine_distance(chroma c1, chroma c2){
	float product=0;
	float root1=0;
	float root2=0;
	int i;
	
	for(i=0;i<CHROMA_SIZE;i++){
	  product += c1.data[i]*c2.data[i];
	  root1 += powf(c1.data[i],2);
	  root2 += powf(c2.data[i],2);
	}
	root1 = sqrtf(root1);
	root2 = sqrtf(root2);
	return product/(root1*root2);
}

/* correlation */
double correlation(double *val, int size, double *profile)
{
  int i;
  double res = 0.0;
      
  for (i=0; i<size; i++)
    {
      res += (val[i]*profile[i]);
    }
  return res;
}

static int freq2chro(double freq, int chroma_size)
{
  return (int)round(chroma_size*log2(freq/261.6256));
}

static int my_modulo(int x, int y)
{
  int res = fmod(x,y);
  if (res <0)
    res+=y;
  return res;
}

chroma* fill_sequence(char* infilename, int *nb_frames)
{
  SNDFILE	 	*infile = NULL ;
  SF_INFO	 	sfinfo ;

  if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
    {	printf ("Not able to open input file %s.\n", infilename) ;
      puts (sf_strerror (NULL)) ;
      return NULL ;
    } ;

  /* Read WAV and Compute Chromas */
  *nb_frames = 0;
  double new_buffer[HOP_SIZE];
  double buffer[FRAME_SIZE];

	
  int i;
  for (i=0;i<(FRAME_SIZE/HOP_SIZE-1);i++)
    {
      if (read_samples (infile, new_buffer, sfinfo.channels)==1)
	fill_buffer(buffer, new_buffer);
      else
	{
	  printf("not enough samples !!\n");
	  return NULL;
	}
    }

  printf("taille : %d échantillons \n", (int)sfinfo.frames);
  int seq_size = (int)ceil((double)sfinfo.frames/HOP_SIZE);
  printf("nbre symboles : %d \n", seq_size);
	
  chroma* sequence1 = malloc(sizeof(struct chroma)*seq_size);
  assert(sequence1);
  
  complex samples[FRAME_SIZE];
  double amplitude[FRAME_SIZE];
  complex spec[FRAME_SIZE];

  const int chroma_size = CHROMA_SIZE;
  double *chroma = malloc(sizeof(double)*chroma_size);
  for (i = 0; i < chroma_size; i++)
    chroma[i] = 0;
	
	

  /* FFT init */
  fft_init(samples, spec);

	
  while (read_samples (infile, new_buffer, sfinfo.channels)==1)
    {
      /* Process Samples */
      //printf("Processing frame %d\n",nb_frames);

      /* hop size */
      fill_buffer(buffer, new_buffer);


      // fft input
      for (i = 0; i < FRAME_SIZE; i++)
	samples[i] = buffer[i];
      
      
      fft_process();

      // amplitude
      for (i = 0; i < FRAME_SIZE; i++)
	{
	  amplitude[i] = cabs(spec[i]);
	}


      /* ---- */
      // contribute chroma
      for (i = 0; i < FRAME_SIZE/2; i++)
	{
	    
	  int bin_chroma = my_modulo(freq2chro((double)i*sfinfo.samplerate/(double)FRAME_SIZE,chroma_size),chroma_size);
	  chroma[bin_chroma] += amplitude[i];
	}

      for (i = 0; i < chroma_size; i++)
	{
	  sequence1[(*nb_frames)].data[i] = chroma[i];
	}
      
      /* reset chroma */
      for (i = 0; i < chroma_size; i++)
	chroma[i] = 0;
	    
      (*nb_frames)++;
    }


  /* FFT exit */
  fft_exit();
  sf_close (infile) ;

  if (!chroma)
    free(chroma);

  return sequence1;
}


int
main (int argc, char * argv [])
{
  char 		*progname, *infilename1, *infilename2;
  int i, j;
  
  progname = strrchr (argv [0], '/') ;
  progname = progname ? progname + 1 : argv [0] ;
  
  if (argc != 3)
    {	print_usage (progname) ;
      return 1 ;
    } ;
  
  infilename1 = argv [1] ;
  infilename2 = argv [2] ;

  int nb_frames1 = 0;
  chroma * sequence1 = fill_sequence(infilename1, &nb_frames1);
  int nb_frames2 = 0;
  chroma * sequence2 = fill_sequence(infilename2, &nb_frames2);

  if (sequence2 == NULL || sequence1 == NULL)
    {
      printf("erreur calcul sequence \n");
      return 1;
    }
  
  for (i = 0; i < nb_frames1; i++)
    {
      for (j = 0; j < CHROMA_SIZE; j++)
	printf("%f - ", sequence1[i].data[j]);
      printf("\n");
    }
  

  for (i = 0; i < nb_frames2; i++)
    {
      for (j = 0; j < CHROMA_SIZE; j++)
	printf("%f - ", sequence2[i].data[j]);
      printf("\n");
    }
  
  /* TODO */
  /* Comparaison */
  for (i = 0; i < 10; i++)
    {
      printf("dist cos : %f \n", cosine_distance(sequence1[i], sequence2[i]));
    }
  
  /* TODO */
  /* Generating PPM Image */
  
  if (!sequence1)
    free(sequence1);
  if (!sequence2)
    free(sequence2);
  
	
  
  return 0 ;
} /* main */

