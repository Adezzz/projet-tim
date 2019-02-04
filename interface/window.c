#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

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
  SDL_Event event;

  SDL_Surface *ecran = NULL;
  //SDL_Surface *rectangle1 = NULL;
  //SDL_Surface *rectangle2 = NULL;
  SDL_Surface *rectangle3 = NULL;
  SDL_Surface *rectangle4 = NULL;
  SDL_Surface *similarite = NULL;
  SDL_Rect /*position1, position2,*/ position3, position4, position_similarite;
  /*position1.x = 20;
  position1.y = 60;
  position2.x = 20;
  position2.y = 200;*/
  position3.x = 20;
  position3.y = 340;
  position4.x = 20;
  position4.y = 480;
  position_similarite.x = 50;
  position_similarite.y = 50;

  if (SDL_Init(SDL_INIT_VIDEO) == -1) // Démarrage de la SDL. Si erreur :
  {
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Écriture de l'erreur
        exit(EXIT_FAILURE); // On quitte le programme
  }

  SDL_WM_SetCaption("In terface d'ANalyse Audio", NULL);

  ecran = SDL_SetVideoMode(1280, 600, 32, SDL_HWSURFACE);
  if (ecran == NULL) // Si l'ouverture a échoué, on le note et on arrête
   {
       fprintf(stderr, "Impossible de charger le mode vidéo : %s\n", SDL_GetError());
       exit(EXIT_FAILURE);
   }
   SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 17, 206, 112)); //changer la couleur

   similarite = IMG_Load("similarityex.jpg");
   SDL_BlitSurface(similarite, NULL, ecran, &position_similarite);

   /*rectangle1 = SDL_CreateRGBSurface(SDL_HWSURFACE, 1240, 100, 32, 0, 0, 0, 0);
   SDL_FillRect(rectangle1, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
   SDL_BlitSurface(rectangle1, NULL, ecran, &position1);

   rectangle2 = SDL_CreateRGBSurface(SDL_HWSURFACE, 1240, 100, 32, 0, 0, 0, 0);
   SDL_FillRect(rectangle2, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
   SDL_BlitSurface(rectangle2, NULL, ecran, &position2);*/

   rectangle3 = SDL_CreateRGBSurface(SDL_HWSURFACE, 1240, 100, 32, 0, 0, 0, 0);
   SDL_FillRect(rectangle3, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
   SDL_BlitSurface(rectangle3, NULL, ecran, &position3);

   rectangle4 = SDL_CreateRGBSurface(SDL_HWSURFACE, 1240, 100, 32, 0, 0, 0, 0);
   SDL_FillRect(rectangle4, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
   SDL_BlitSurface(rectangle4, NULL, ecran, &position4);

    SDL_Flip(ecran);
    pause();

    //SDL_FreeSurface(rectangle1);
    //SDL_FreeSurface(rectangle2);
    SDL_FreeSurface(similarite);
    SDL_FreeSurface(rectangle3);
    SDL_FreeSurface(rectangle4);
    SDL_Quit();

    return EXIT_SUCCESS;
}
