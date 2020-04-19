#include "settings.h"

static settings programSettings;

void setDebug(bool debugSetting)
{
  programSettings.debug = debugSetting;
}

void setFrequency(int freqSetting)
{
  programSettings.max_frequency = freqSetting;
}

void setFramerate(int frameSetting)
{
  programSettings.max_framerate = frameSetting;
}

bool getDebugMode()
{
  return programSettings.debug;
}

int getFrequency()
{
  return programSettings.max_frequency;
}

int getFramerate()
{
  return programSettings.max_framerate;
}

bool *getGUIPtr()
{
    return &programSettings.showGUI;
}
