#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdbool.h>

typedef struct runtime_settings
{
  bool debug;
  int max_frequency;
  int max_framerate;
  bool showGUI;
} chip8settings;

void setDebug(bool debugSetting);

bool getDebugMode();
#endif
