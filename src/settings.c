#include "settings.h"

static chip8settings programSettings;

void setDebug(bool debugSetting)
{
  programSettings.debug = debugSetting;
}

bool getDebugMode()
{
  return programSettings.debug;
}
