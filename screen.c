#include "screen.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int initScreen()
{
  SDL_Window* window = NULL;
  SDL_Surface* surface = NULL;

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL Could not initialize: %s\n", SDL_GetError());
    return -1;
  } else
  {
    window = SDL_CreateWindow("Chip8 Interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
      printf("SDL Could not create window %s\n", SDL_GetError());
      return -1;
    } else
    {
      surface = SDL_GetWindowSurface(window);

      SDL_Delay(50);

      SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
      SDL_UpdateWindowSurface(window);

      SDL_Delay(3000);
      SDL_DestroyWindow(window);
      SDL_Quit();

      return 0;
    }
  }
}
