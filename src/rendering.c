#include "rendering.h"

struct cacheNode
{
  SDL_Texture *texture;
  int key;
  struct cacheNode *next;
  struct cacheNode *previous;
};

typedef struct cache
{
  int capacity;
  int count;
  struct cacheNode *head;
  struct cacheNode *tail;
} LRUCache;



SDL_Texture* renderText(char* text, SDL_Color color, SDL_Renderer *renderer, int fontSize)
{
  static bool first = true;
  static TTF_Font *font;
  if(first)
  {
    font = TTF_OpenFont("./fonts/slkscrb.ttf", fontSize);
    if(font == NULL)
    {
      printf("Error opening font file.");
    }
  }

  SDL_Surface *textSurf = TTF_RenderText_Solid(font, text, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurf);

  SDL_FreeSurface(textSurf);
  return texture;
}

void drawTexture(SDL_Texture *tex, SDL_Renderer *renderer, int x, int y)
{
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;

  SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
  SDL_RenderCopy(renderer, tex, NULL, &dst);
}

void drawDebug(int x, int y, Chip8State *cpu, SDL_Renderer *renderer)
{
  int i;
  uint8_t displayedInstructions[10];
  SDL_Texture *opcodeText[10];
  SDL_Color textColor;
  textColor.r = 0;
  textColor.b = 0;
  textColor.g = 0;
  textColor.a = 255;

  for(i = 0; i < 10 && (i + cpu->stackPointer) < 4096; i++) // Get opcodes at next ten memory addresses.
  {
    displayedInstructions[i] = cpu->memory[i+cpu->stackPointer];
    char textBuffer [50]; 
    sprintf(textBuffer, "%X", displayedInstructions[i]);
    opcodeText[i] = renderText(textBuffer, textColor, renderer, 16);
  }
}
