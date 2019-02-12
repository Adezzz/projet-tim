#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NB_CHAR 100

enum extension{
  MIDI,
  WAV,
  MP3
};

/* Checks file extension. If != 0, strings are different. Else, they are equal. */
short check_ext(const char *str, const unsigned int size, const enum extension ext){
  char ext_str[][4] = {".mid", ".wav", ".mp3"};
  return strncmp(str + (size - 4), ext_str[ext], 4);
}


int main(int argc, char *argv[]){

  printf("##################################\n");
  printf("#### ANALYSE AUDIO\n");
  printf("##################################\n");
  printf("Entrer un numéro de commande:\n");
  printf("1. Extraction de rythme.\n");
  printf("2. Calcul de similarité (MFCC).\n");
  printf("3. Estimation de la tonalité.\n");

  char choix = getchar();
  int num = choix - '0';
  char fichier[NB_CHAR];
  char fichier2[NB_CHAR];
  char commande[NB_CHAR*3];
  enum extension ext;

  switch (num) {
    case 1:
      printf("Chemin de l'extrait à analyser:\n");
      scanf("%s", fichier);
      ext = WAV;
      if (check_ext(fichier, strlen(fichier), ext) != 0) {
        printf("ERREUR: fichier WAV attendu.\n");
        return EXIT_FAILURE;
      }

      strcpy(commande, "./rythme ");
      strncat(commande, fichier, 100);
      strcat(commande, " rythme_output.wav");

      system(commande);
      printf("Fichier créé: rythme_output.wav\n");
      break;

    case 2:

      printf("Chemin du premier extrait WAV à analyser:\n");
      scanf("%s", fichier);
      printf("Chemin du second extrait WAV à analyser:\n");
      scanf("%s", fichier2);

      strcpy(commande, "./similar ");
      strncat(commande, fichier, 100);
      strcat(commande, " ");
      strncat(commande, fichier2, 100);

      printf("Calcul des coefficients MFCC...\n");
      printf("Comparaison des extraits...\n");
      printf("\n***\n");

      system(commande);
      printf("Fichier créé: rythme_output.wav\n");
      break;

    case 3:
      printf("\nVous avez choisi: %d\n", choix);
      break;
    default:
      printf("\nVeuillez entrer un nombre entre 1 et 3.\n");
      break;
  }


  return EXIT_SUCCESS;
}
