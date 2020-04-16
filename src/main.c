#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include "settings.h"
#include "interpret.h"
#include "screen.h"

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
  initialize(&state);
  clock_t cpuClock = clock();
  clock_t renderClock = clock();
  srand(clock());

  if(argc < 2)
  {
    printf("Please specify an input file.\n");
    return 0;
  }

  // Set default clock speed and maximum framerate
  setFrequency(500);
  setFramerate(60);

  for(int i = 1; i < argc-1; i++) // Replace with open-source CLI argument parsing lib at first convenience.
  {
    if(strcmp(argv[i], "-d") == 0)
    {
      setDebug(true);
    }

    if(strcmp(argv[i], "-f") == 0)
    {
      int framerate;
      if(i + 1 < argc-1)
      {
        sscanf(argv[i+1], "%d", &framerate);
        setFramerate(framerate);
        printf("Setting framerate to %d.\n", framerate);
      } else
      {
        printf("-f predicate requires numerical argument.\n");
      }
    }

    if(strcmp(argv[i], "-h") == 0)
    {
      int frequency;
      if(i + 1 < argc-1)
      {
        sscanf(argv[i+1], "%d", &frequency);
        setFrequency(frequency);
        printf("Setting clock frequency to %d Hertz.\n", frequency);
      } else
      {
        printf("-h predicate requires numerical argument.\n");
      }
    }
  }
  char* filename = argv[argc-1];
  loadProgram(&state, filename);

  int rendererState = 0;

  if(initScreen() != 0)
  {
    printf("Unable to initialize SDL\n");
    return -1;
  }

  bool running = true;
  while(running)
  {
    pollEvents();
    if((double)(clock() - cpuClock)/CLOCKS_PER_SEC*1000 > (1000/getFrequency()))
    {
      if(run(&state) == -1)
      {
        printf("Invalid opcode, exiting.\n");
        return -1;
      }
      cpuClock = clock();
    }
    if((double)(clock() - renderClock)/CLOCKS_PER_SEC*1000 > (1000/getFramerate()))
    {
        renderClock = clock();
        renderFrame(state.screen);
    }
  }
  cleanupSDL();

  return 0;
}
