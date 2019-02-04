#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

void pause()
{
    int continuer = 1;
    SDL_Event event;

    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
        }
    }
}


 int main(int argc, char *argv[])
{
  SDL_Surface *ecran = NULL;
  SDL_Surface *rectangle = NULL;
  SDL_Rect position;
  position.x = 10;
  position.y = 10;

  if (SDL_Init(SDL_INIT_VIDEO) == -1) // Démarrage de la SDL. Si erreur :
  {
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Écriture de l'erreur
        exit(EXIT_FAILURE); // On quitte le programme
  }

  SDL_WM_SetCaption("In terface d'ANalyse Audio", NULL);

  ecran = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
  if (ecran == NULL) // Si l'ouverture a échoué, on le note et on arrête
   {
       fprintf(stderr, "Impossible de charger le mode vidéo : %s\n", SDL_GetError());
       exit(EXIT_FAILURE);
   }
   SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 17, 206, 112)); //changer la couleur

   rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, 220, 180, 32, 0, 0, 0, 0);
   SDL_FillRect(rectangle, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
   SDL_BlitSurface(rectangle, NULL, ecran, &position);

    SDL_Flip(ecran);
    pause();

    SDL_FreeSurface(rectangle);
    SDL_Quit();

    return EXIT_SUCCESS;
}
