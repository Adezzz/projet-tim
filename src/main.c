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
  printf("####       ANALYSE AUDIO      ####\n");
  printf("##################################\n");
  printf("Entrez un numéro de commande:\n");
  printf("1. Extraction de rythme (WAV).\n");
  printf("2. Calcul de similarité avec coefficients MFCC (WAV).\n");
  printf("3. Estimation de la tonalité (MIDI).\n");
  printf("4. Affichage des informations audio (WAV).\n");
  printf("5. Visualisation de matrice d'autosimilarité (WAV).\n");

  printf("6. Quitter.\n");

  char choix = getchar();
  int res;
  int num = choix - '0';
  char fichier[NB_CHAR];
  char fichier2[NB_CHAR];
  char commande[NB_CHAR*3];
  enum extension ext;

  switch (num) {
    case 1:
      printf("Chemin de l'extrait à analyser:\n");
      if ((res=scanf("%s", fichier))<0)
        return EXIT_FAILURE;
      ext = WAV;
      if (check_ext(fichier, strlen(fichier), ext) != 0) {
        printf("ERREUR: fichier WAV attendu.\n");
        return EXIT_FAILURE;
      }

      strcpy(commande, "bin/rythme ");
      strncat(commande, fichier, 100);
      strcat(commande, " rythme_output.wav");

      if ((res=system(commande)) == -1)
        return EXIT_FAILURE;
      printf("Fichier créé: rythme_output.wav\n");
      break;

    case 2:

      printf("Chemin du premier extrait WAV à analyser:\n");
      if ((res=scanf("%s", fichier))<0)
        return EXIT_FAILURE;
      printf("Chemin du second extrait WAV à analyser:\n");
      if ((res=scanf("%s", fichier2))<0)
        return EXIT_FAILURE;

      ext = WAV;
      if (check_ext(fichier, strlen(fichier), ext) != 0 || check_ext(fichier2, strlen(fichier2), ext) != 0) {
        printf("ERREUR: fichier WAV attendu.\n");
        return EXIT_FAILURE;
      }

      strcpy(commande, "bin/similar ");
      strncat(commande, fichier, 100);
      strcat(commande, " ");
      strncat(commande, fichier2, 100);

      printf("Calcul des coefficients MFCC...\n");
      printf("Comparaison des extraits...\n");
      printf("\n***\n");

      if ((res=system(commande)) == -1)
        return EXIT_FAILURE;
      break;

    case 3:
      printf("Chemin de l'extrait à analyser:\n");
      if ((res=scanf("%s", fichier))<0)
        return EXIT_FAILURE;
      ext = MIDI;
      if (check_ext(fichier, strlen(fichier), ext) != 0) {
        printf("ERREUR: fichier MIDI attendu.\n");
        return EXIT_FAILURE;
      }

      strcpy(commande, "bin/ton ");
      strncat(commande, fichier, 100);
      printf("\n***\n");

      if ((res=system(commande)) == -1)
        return EXIT_FAILURE;
      break;


    case 4:
      printf("Chemin du premier extrait WAV à analyser:\n");
      if ((res=scanf("%s", fichier))<0)
        return EXIT_FAILURE;

      strcpy(commande, "bin/info ");
      strncat(commande, fichier, 100);

      if ((res=system(commande)) == -1)
        return EXIT_FAILURE;
      break;

    case 5:
      printf("Chemin de l'extrait à analyser:\n");
      if ((res=scanf("%s", fichier))<0)
        return EXIT_FAILURE;
      ext = WAV;
      if (check_ext(fichier, strlen(fichier), ext) != 0) {
        printf("ERREUR: fichier WAV attendu.\n");
        return EXIT_FAILURE;
      }

      strcpy(commande, "bin/autocor ");
      strncat(commande, fichier, 100);
      strcat(commande, " ");
      strncat(commande, fichier, 100);

      if ((res=system(commande)) == -1)
        return EXIT_FAILURE;
      printf("Matrice de corrélation sauvegardée dans: sim_matrix.ppm\n");
      if ((res=system("eog sim_matrix.ppm")) == -1)
        return EXIT_FAILURE;
      break;

    case 6:
      printf("Au revoir !\n");
      exit(0);
      break;

    default:
      printf("\nVeuillez entrer un nombre entre 1 et 3.\n");
      break;
  }


  return EXIT_SUCCESS;
}
