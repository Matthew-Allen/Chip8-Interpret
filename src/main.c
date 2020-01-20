#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "interpret.h"

#define MAX_PROGRAM_SIZE ( PROGRAM_MEMORY_END - PROGRAM_MEMORY_START )

int loadProgram(Chip8State* cpu, char* path)
{
  FILE* programFile = fopen(path, "r");

  if(programFile == NULL)
  {
    printf("Error opening file %s\n", path);
    return -1;
  }

  int bytesRead = fread((cpu->memory + 0x200), 1, MAX_PROGRAM_SIZE + 1, programFile);
  printf("Finished reading file \"%s\"\n%d bytes loaded.\n", path, bytesRead);

  if(bytesRead > MAX_PROGRAM_SIZE)
  {
    printf("Warning: Read stopped before reaching EOF (File size too large).\n");
    return -1;
  }
  return 0;
}

int main(int argc, char* argv[])
{

  Chip8State state;
  if(argc == 2)
  {
    char* filename = argv[1];
    loadProgram(&state, filename);
  }

  return 0;
}
