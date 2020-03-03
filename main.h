#ifndef __MAIN_H__
#define __MAIN_H__

#include <SDL2/SDL.h>

typedef struct
{
    CONTROL *control;               // pointer to CONTROL struct in Core library
    BUTTONS buttons;
    SDL_GameController *gamepad;
} SController;

extern "C"
{
    EXPORT void CALL Config_DoConfig();
}

#endif
