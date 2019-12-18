#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "interpret.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


int main(int argc, char* argv[])
{


  // Perform SDL initialization
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL Could not initialize: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Window *renderWindow = SDL_CreateWindow("Chip8 Interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if(renderWindow == NULL)
  {
    printf("SDL Could not create window %s\n", SDL_GetError());
    return -1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(renderWindow, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == NULL)
  {
    printf("SDL could not create renderer: %s\n", SDL_GetError());
  }
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


  // initialize TTF
  TTF_Init();
  SDL_Delay(50);


  // init vars for drawing screen background and polling events.
  SDL_Event event;
  SDL_Rect background;
  background.x = 0;
  background.y = 0;
  background.w = SCREEN_WIDTH;
  background.h = SCREEN_HEIGHT;

  while(1)
  {
   
    if(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        printf("Exiting.\n");
        break;
      }
    }

    SDL_RenderClear(renderer);
    SDL_RenderFillRect(renderer, &background);
    SDL_RenderPresent(renderer);
 }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(renderWindow);
  TTF_Quit();
  SDL_Quit();

  return 0;
}

void quitScreen()
{
}
