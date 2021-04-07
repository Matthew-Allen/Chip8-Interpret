#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <libconfig.h>
#include "interpret.h"
#include "screen.h"


int main(int argc, char* argv[])
{

    Chip8State* state;
    state = createDefaultState();
    initialize(state);
    struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 1L;
    struct timespec cpuTime, renderTime, diffTime, result;
    srand(clock());
    unsigned int framerate;
    config_t configData;
    
    //Load data from configuration file.
    config_init(&configData);
    if(!config_read_file(&configData,"config.ini"))
    {
        printf("Failed to open config file: %s\n",config_error_text(&configData));
        return -1;
    }

    if(argc < 2)
    {
        printf("Please specify an input file.\n");
        return 0;
    }

    // Set default clock speed and maximum framerate
    state->frequency = 500;
    framerate = 60;

    for(int i = 1; i < argc-1; i++) // Replace with open-source CLI argument parsing lib at first convenience.
    {

        if(strcmp(argv[i], "-f") == 0)
        {
            if(i + 1 < argc-1)
            {
                sscanf(argv[i+1], "%d", &framerate);
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
                state->frequency = frequency;
                printf("Setting clock frequency to %d Hertz.\n", frequency);
            } else
            {
                printf("-h predicate requires numerical argument.\n");
            }
        }
    }
    char* filename = argv[argc-1];
    loadProgram(state, filename);

    int rendererState = 0;

    if(initScreen() != 0)
    {
        printf("Unable to initialize SDL\n");
        return -1;
    }

    bool running = true;
    clock_gettime(CLOCK_MONOTONIC, &cpuTime);
    clock_gettime(CLOCK_MONOTONIC, &renderTime);
    while(running)
    {

        running = pollEvents(&configData);
        clock_gettime(CLOCK_MONOTONIC, &diffTime);
        timediff(&result, &diffTime, &cpuTime);
        if((result.tv_nsec / (double) MILLION) > (1000/(double)state->frequency) && state->paused == false)
        {
            int opResult = run(state);
            clock_gettime(CLOCK_MONOTONIC, &cpuTime);
        }

        timediff(&result, &diffTime, &renderTime);
        if((result.tv_nsec /(double) MILLION) > (1000/(double)framerate))
        {
            clock_gettime(CLOCK_MONOTONIC, &renderTime);
            renderFrame(state, &configData);
        }
        usleep(1);
    }
    remove("config.ini");
    config_write_file(&configData, "config.ini");
    cleanupSDL();

    return 0;
}
