#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <stdbool.h>


typedef struct {
  SDL_Rect draw_rect;    // dimensions of button
  struct {
    Uint8 r, g, b, a;
  } colour;

  bool pressed;
} button_t;

static void button_process_event(button_t *btn, const SDL_Event *ev) {
  // react on mouse click within button rectangle by setting 'pressed'
  if(ev->type == SDL_MOUSEBUTTONDOWN) {
    if(ev->button.button == SDL_BUTTON_LEFT &&
      ev->button.x >= btn->draw_rect.x &&
      ev->button.x <= (btn->draw_rect.x + btn->draw_rect.w) &&
      ev->button.y >= btn->draw_rect.y &&
      ev->button.y <= (btn->draw_rect.y + btn->draw_rect.h)) {
        btn->pressed = true;
      }
    }
  }

  int main(int argc, char *argv[])
  {
    SDL_Surface *ecran = NULL;
    SDL_Surface *bouton1 = NULL;
    SDL_Surface *bouton2 = NULL;
    SDL_Event event;

    SDL_Rect position;
    position.x = 640/2 - 75;
    position.y = 480/2 - 20;

    if (SDL_Init(SDL_INIT_VIDEO) == -1) // Démarrage de la SDL. Si erreur :
    {
      fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Écriture de l'erreur
      exit(EXIT_FAILURE); // On quitte le programme
    }

    SDL_WM_SetCaption("Analyse Audio", NULL);

    ecran = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
    if (ecran == NULL) // Si l'ouverture a échoué, on le note et on arrête
    {
      fprintf(stderr, "Impossible de charger le mode vidéo : %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 17, 206, 112)); //changer la couleur

    //Buttons
    button_t start_button = {
      .colour = { .r = 255, .g = 255, .b = 255, .a = 255, },
      .draw_rect = { .x = position.x, .y = position.y, .w = 150, .h = 40 },
    };
    bouton1 = SDL_CreateRGBSurface(SDL_HWSURFACE, start_button.draw_rect.w, start_button.draw_rect.h, 32, start_button.colour.r, start_button.colour.g, start_button.colour.b, start_button.colour.a);
    SDL_FillRect(bouton1, NULL, SDL_MapRGB(ecran->format, start_button.colour.r, start_button.colour.g, start_button.colour.b));
    SDL_BlitSurface(bouton1, NULL, ecran, &position);

    bool quit = false;

    while(!quit)
    {
      SDL_WaitEvent(&event);
      if(event.type == SDL_QUIT) {
        quit = true;
        break;
      }

      button_process_event(&start_button, &event);
      if (start_button.pressed  == true)
      {
        printf("Button1 pressed\n");
        /* APPLY PROGRAM*/
        start_button.colour.r = 0;
        bouton1 = SDL_CreateRGBSurface(SDL_HWSURFACE, start_button.draw_rect.w, start_button.draw_rect.h, 32, start_button.colour.r, start_button.colour.g, start_button.colour.b, start_button.colour.a);
        SDL_FillRect(bouton1, NULL, SDL_MapRGB(ecran->format, start_button.colour.r, start_button.colour.g, start_button.colour.b));
        SDL_BlitSurface(bouton1, NULL, ecran, &position);
        start_button.pressed = false;
      }
      SDL_Flip(ecran);
    }


    SDL_FreeSurface(bouton1);
    SDL_Quit();

    return EXIT_SUCCESS;
  }
