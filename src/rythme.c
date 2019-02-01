#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sndfile.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>
#include "../lib/gnuplot_i.h"

#define	FRAME_SIZE 512
#define HOP_SIZE 256

static gnuplot_ctrl *h;
static fftw_plan plan = NULL;


static void print_usage (char *progname) {

	printf ("\nUsage : %s <input file> <output file>\n", progname) ;
	puts ("\n") ;
}


static void fill_buffer(double *buffer, double *new_buffer) {

	int i;
	double tmp[FRAME_SIZE-HOP_SIZE];

	/* save */
	for (i=0;i<FRAME_SIZE-HOP_SIZE;i++)
	tmp[i] = buffer[i+HOP_SIZE];

	/* save offset */
	for (i=0;i<(FRAME_SIZE-HOP_SIZE);i++) {
		buffer[i] = tmp[i];
	}

	for (i=0;i<HOP_SIZE;i++) {
		buffer[FRAME_SIZE-HOP_SIZE+i] = new_buffer[i];
	}
}


static int read_n_samples (SNDFILE * infile, double * buffer, int channels, int n) {

	if (channels == 1) {
		/* MONO */
		int readcount ;
		readcount = sf_readf_double (infile, buffer, n);
		return readcount==n;

	}	else if (channels == 2)	{
		/* STEREO */
		double buf [2 * n] ;
		int readcount, k ;
		readcount = sf_readf_double (infile, buf, n);
		for (k = 0 ; k < readcount ; k++)
		buffer[k] = (buf [k * 2]+buf [k * 2+1])/2.0 ;
		return readcount==n;

	}	else	{
		/* FORMAT ERROR */
		printf ("Channel format error.\n");
	}

	return 0;
}


static int write_n_samples (SNDFILE * outfile, double * buffer, int channels, int n) {

	if (channels == 1) {
		/* MONO */
		int writecount ;
		writecount = sf_writef_double (outfile, buffer, n);
		return writecount==n;
	}	else {
		/* FORMAT ERROR */
		printf ("Channel format output error.\n");
	}
	return 0;
}


static int read_samples (SNDFILE * infile, double * buffer, int channels) {

	return read_n_samples (infile, buffer, channels, HOP_SIZE);
}


static int write_samples (SNDFILE * outfile, double * buffer, int channels) {

	return write_n_samples (outfile, buffer, channels, HOP_SIZE);
}


void fft_init (complex in[FRAME_SIZE], complex spec[FRAME_SIZE]) {

	plan = fftw_plan_dft_1d (FRAME_SIZE, in, spec, FFTW_FORWARD, FFTW_ESTIMATE);
}


void fft_exit (void) {

	fftw_destroy_plan (plan);
}


void fft_process (void) {

	fftw_execute (plan);
}


int main (int argc, char * argv []) {

	char 		*progname, *infilename, *outfilename;
	SNDFILE	 	*infile = NULL;
	SNDFILE		*outfile = NULL;
	SF_INFO	 	sfinfo;

	progname = strrchr (argv [0], '/');
	progname = progname ? progname + 1 : argv [0];

	if (argc != 3) {
		print_usage (progname);
		return 1 ;
	}

	infilename = argv [1];
	outfilename = argv [2];

	if (strcmp (infilename, outfilename) == 0) {
		printf ("Error : Input and output filenames are the same.\n\n");
		print_usage (progname);
		return 1;
	}


	if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL) {
		printf ("ERROR: couldn't open input file.\n");
		perror("wrong_input");
		return EXIT_FAILURE;
	}

	int nb_frames_in = (int)sfinfo.frames/HOP_SIZE;

	/* Open the output file. */
	if ((outfile = sf_open (outfilename, SFM_WRITE, &sfinfo)) == NULL) {
		printf ("ERROR: couldn't open input file.\n");
		perror("wrong_input");
		return EXIT_FAILURE;
	}

	int nb_frames = 0;
	int i = 0;
	double new_buffer[HOP_SIZE];
	double buffer[FRAME_SIZE];
	/* Read WAV */
	for (i = 0; i < (FRAME_SIZE/HOP_SIZE-1); i++)	{
		if (read_samples (infile, new_buffer, sfinfo.channels) == 1) {
			fill_buffer(buffer, new_buffer);
		}	else {
			printf("Not enough samples !!\n");
			return 1;
		}
	}

	complex samples[FRAME_SIZE];
	double amplitude[FRAME_SIZE];
	double amplitude_prev[FRAME_SIZE];
	complex spec[FRAME_SIZE];
	double spectralFlux[nb_frames_in];
	double flux = 0.0;

	for (i = 0; i < FRAME_SIZE; i++)
		amplitude_prev[i] = 0.0;

	/* Plot init */
	h = gnuplot_init();
	gnuplot_setstyle(h, "lines");

	/* FFT init */
	fft_init(samples, spec);

	while (read_samples (infile, new_buffer, sfinfo.channels) == 1)	{

		printf("Processing frame %d\n", nb_frames);
		fill_buffer(buffer, new_buffer); /* hop size */

		for (i = 0; i < FRAME_SIZE; i++) // fft input
			samples[i] = buffer[i];

		fft_process();

		for (i = 0; i < FRAME_SIZE; i++) { // amplitude
			amplitude_prev[i] = amplitude[i];
			amplitude[i] = cabs(spec[i]);
		}

		flux = 0.0; // flux spectral

		for (i = 0; i < FRAME_SIZE; i++) {
			double amp_diff = amplitude[i] - amplitude_prev[i];
			if (amp_diff > 0) flux += amp_diff;
		}

		flux /= (double)FRAME_SIZE;

		// If over threshold, synthesize sin() sound.
		if (flux > 0.3)	{
			spectralFlux[nb_frames] = flux;
			for (i = 0; i < FRAME_SIZE; i++) {
				buffer[i] += (0.25 * sin(2*M_PI*1760.0*(double)i/sfinfo.samplerate)) * (0.5 - 0.5 * cos(2.0*M_PI*(double)i/FRAME_SIZE));
			}

		}	else {
			spectralFlux[nb_frames] = 0.0;
		}

		write_samples (outfile, buffer, sfinfo.channels);
		nb_frames++;
	}

	/* PLOT */
	gnuplot_resetplot(h);
	gnuplot_plot_x(h, spectralFlux, nb_frames, "Spectral Flux");
	sleep(2);

	/* TEMPO ESTIMATION */
	double gamma[nb_frames_in];
	int tau;

	for (tau = 0; tau < nb_frames_in; tau++) {
		gamma[tau] = 0;
		for (i = 0; i < nb_frames_in - tau; i++)
			gamma[tau] += (spectralFlux[i] * spectralFlux[i+tau]) / (double)nb_frames_in;
	}

	int imax=0;
	double autoc_max = 0.0; // Autocorrelation
	for (i = 20; i < 200; i++) {
		if (gamma[i] > autoc_max)	{
			autoc_max = gamma[i];
			imax = i;
		}
}

	printf("Maximum autocorrelation = %d\n", imax);
	printf("Frequency (Hz) = %f\n", 44100.0 / ((double)imax * HOP_SIZE));
	printf("Period (s) = %f\n", (double)imax * HOP_SIZE / 44100.0);
	printf("Tempo (BPM) = %f\n", 60.0 / ((double)imax * HOP_SIZE / 44100.0));


	// Peigne (déphasage)
	double comb[nb_frames_in];

	for (i=0; i < nb_frames_in; i++) {
		if (i % imax == 0) {
			comb[i] = 1;
		}	else {
			comb[i] = 0;
		}
	}

	for (tau = 0; tau < nb_frames_in; tau++) {
		gamma[tau]=0;
		for (i = 0; i < nb_frames_in-tau; i++)
			gamma[tau] += (spectralFlux[i]*comb[i+tau])/(double)nb_frames_in;
	}

	int comb_imax=0;
	double comb_max = 0.0;

	for (i=0; i < imax; i++) {
		if (gamma[i] > comb_max)
		{
			comb_max = gamma[i];
			comb_imax = i;
		}
	}

	printf("Max intercorrelation %d\n", comb_imax);

	/* PLOT */
	gnuplot_resetplot(h);
	gnuplot_plot_x(h, gamma, nb_frames_in, "Comb");
	sleep(2);
	sf_close (infile);

	/* Synthesis beat tracker */
	//Correcting the beats synthesized previously out of place

	if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL) {
		printf ("ERROR: couldn't open input file.\n");
		perror("wrong_input");
		return EXIT_FAILURE;
	}

	if ((outfile = sf_open (outfilename, SFM_WRITE, &sfinfo)) == NULL) {
		printf ("ERROR: couldn't open input file.\n");
		perror("wrong_input");
		return EXIT_FAILURE;
	}

	for (i = 0; i < (FRAME_SIZE / HOP_SIZE - 1); i++) {

		if (read_samples (infile, new_buffer, sfinfo.channels) == 1) {
			fill_buffer(buffer, new_buffer);
		} else {
			printf("ERROR: not enough samples provided.\n");
			perror("sample_lack");
			return EXIT_FAILURE;
		}
	}

	nb_frames = 0;
	printf("Adjusting...\n");
	while (read_samples (infile, new_buffer, sfinfo.channels) == 1) {

		fill_buffer(buffer, new_buffer);
		int select = 1;	// Boolean flag to select only 1 beat out of 2
		if (nb_frames % imax == (imax - comb_imax) && select == 1)	{ // If found beat, synthesize sin()
			for (i = 0; i < FRAME_SIZE; i++) {
				buffer[i] += (0.35 * sin(2*M_PI*1760.0*(double)i/sfinfo.samplerate)) *	(0.5-0.5*cos(2.0*M_PI*(double)i/FRAME_SIZE));;
			}
		} else if (select == 0) {
			select = 1;
		}

		write_samples (outfile, buffer, sfinfo.channels);
		nb_frames++;
	}

	printf("Done.\n");
	sf_close (infile);
	sf_close (outfile);

	/* FFT exit */
	fft_exit();

	return 0;
}
