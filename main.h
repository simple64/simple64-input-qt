#ifndef __MAIN_H__
#define __MAIN_H__

typedef struct
{
    CONTROL *control;               // pointer to CONTROL struct in Core library
    BUTTONS buttons;
} SController;

extern "C"
{
    EXPORT void CALL Config_DoConfig();
}

#endif
