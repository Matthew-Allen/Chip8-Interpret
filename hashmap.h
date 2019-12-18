#ifndef HASHMAP_H
#define HASHMAP_H

#include <SDL2/SDL.h>

#define DEFAULT_HASHMAP_SIZE 200


typedef struct mapNode 
{
  long key;
  SDL_Texture *tex;
  struct mapNode *prev;
  struct mapNode *next;
} mapNode;

typedef struct map
{
  mapNode *buckets[DEFAULT_HASHMAP_SIZE];
} hashMap;

SDL_Texture* getByString(hashMap *map, char* hashString);

#endif
