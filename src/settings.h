#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdbool.h>

typedef struct runtime_settings
{
  bool debug;
  int max_frequency;
  int max_framerate;
} settings;

void setDebug(bool debugSetting);

void setFrequency(int freqSetting);

void setFramerate(int frameSetting);

bool getDebugMode();

int getFrequency();

int getFramerate();

#endif
