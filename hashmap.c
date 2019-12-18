#include "hashmap.h"

unsigned long djb2Hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

mapNode* findNode(hashMap *map, long key)
{
  long bucket = key % DEFAULT_HASHMAP_SIZE;
  mapNode *foundNode = map->buckets[bucket];

  while(foundNode->key != key)
  {
    foundNode = foundNode->next;
    if(foundNode == NULL)
    {
      return NULL;
    }
  }

  return foundNode;
}


void insertKey(hashMap *map, SDL_Texture *texture, char *hashString)
{
  mapNode *newNode = malloc(sizeof(mapNode));
  long newKey = djb2Hash(hashString);
  long bucket = newKey % DEFAULT_HASHMAP_SIZE;
  newNode->tex = texture;
  newNode->key = newKey;

  if(map->buckets[bucket] != NULL) // If the current bucket is not occupied, add the new node pointer to the base index.
  {
    map->buckets[bucket] = newNode;
  } else
  {
    mapNode *currentNode = map->buckets[bucket]; // Otherwise, keep note of pointer in the bucket...
    while(currentNode->next != NULL) // And while that pointer's next node isn't null
    {
      currentNode = currentNode->next; // Set it equal to the next node
    }
    currentNode->next = newNode; // And once we find a node with a NULL next pointer, store the new node pointer there.
  }
}

void removeKey(hashMap *map, char* hashString) // Remove node at key created from the hash string
{
  long newKey = djb2Hash(hashString);
  long bucket = newKey % DEFAULT_HASHMAP_SIZE;
  mapNode *foundNode;

  foundNode = findNode(map, newKey);

  if(foundNode->prev != NULL)
  {
    foundNode->prev->next = foundNode->next;
    if(foundNode->next != NULL)
    {
      foundNode->next->prev = foundNode->prev;
    }
  } else
  {
    map->buckets[bucket] = foundNode->next;
    map->buckets[bucket]->prev = NULL;
  }
  SDL_DestroyTexture(foundNode->tex);
  free(foundNode);
}

SDL_Texture* getByString(hashMap *map, char* hashString) // Note: Returns NULL texture if no node is found.
{
  long key = djb2Hash(hashString);
  mapNode *node = findNode(map, key);
  if(node != NULL)
  {
    return node->tex;
  } else
  {
    return NULL;
  }
}

