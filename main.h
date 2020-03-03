#ifndef __MAIN_H__
#define __MAIN_H__

#include <QString>
#include <SDL2/SDL.h>

typedef struct
{
    CONTROL *control;               // pointer to CONTROL struct in Core library
    SDL_GameController *gamepad;
    QString profile;
} SController;

extern "C"
{
    EXPORT void CALL Config_DoConfig();
}

#endif
