#define M64P_PLUGIN_PROTOTYPES 1
#include "m64p_common.h"
#include "m64p_types.h"
#include "m64p_config.h"
#include "m64p_plugin.h"
#include "main.h"
#include "configdialog.h"
#include "osal/osal_dynamiclib.h"

#include <QDir>
#include <SDL2/SDL.h>

#define QT_INPUT_PLUGIN_VERSION 0x020500
#define INPUT_PLUGIN_API_VERSION 0x020100
static int l_PluginInit = 0;
static unsigned char myKeyState[SDL_NUM_SCANCODES];
QSettings* settings;
SController controller[4];   // 4 controllers

Q_DECLARE_METATYPE(QList<int>)

EXPORT m64p_error CALL PluginStartup(m64p_dynlib_handle CoreHandle, void *, void (*)(void *, int, const char *))
{
    if (l_PluginInit)
        return M64ERR_ALREADY_INIT;

    ptr_ConfigGetUserConfigPath ConfigGetUserConfigPath = (ptr_ConfigGetUserConfigPath) osal_dynlib_getproc(CoreHandle, "ConfigGetUserConfigPath");
    QDir ini_path(ConfigGetUserConfigPath());
    settings = new QSettings(ini_path.absoluteFilePath("input-profiles.ini"), QSettings::IniFormat);

    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");

    QList<int> values;
    QString section = "Auto-Keyboard";
    values.insert(0, 0/*blank value*/);
    values.insert(1, 0/*Keyboard*/);
    if (!settings->contains(section)) {
        values.replace(0, SDL_SCANCODE_LSHIFT);
        settings->setValue(section + "/A", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_LCTRL);
        settings->setValue(section + "/B", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_Z);
        settings->setValue(section + "/Z", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_X);
        settings->setValue(section + "/L", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_C);
        settings->setValue(section + "/R", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_RETURN);
        settings->setValue(section + "/Start", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_A);
        settings->setValue(section + "/DPadL", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_D);
        settings->setValue(section + "/DPadR", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_W);
        settings->setValue(section + "/DPadU", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_S);
        settings->setValue(section + "/DPadD", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_J);
        settings->setValue(section + "/CLeft", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_L);
        settings->setValue(section + "/CRight", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_I);
        settings->setValue(section + "/CUp", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_K);
        settings->setValue(section + "/CDown", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_LEFT);
        settings->setValue(section + "/AxisLeft", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_RIGHT);
        settings->setValue(section + "/AxisRight", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_UP);
        settings->setValue(section + "/AxisUp", QVariant::fromValue(values));
        values.replace(0, SDL_SCANCODE_DOWN);
        settings->setValue(section + "/AxisDown", QVariant::fromValue(values));

        settings->setValue(section + "/Deadzone", 12.5);
    }

    section = "Auto-Gamepad";
    values.replace(1, 1/*Gamepad*/);
    if (!settings->contains(section)) {
        values.replace(0, SDL_CONTROLLER_BUTTON_A);
        settings->setValue(section + "/A", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_BUTTON_X);
        settings->setValue(section + "/B", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        values.insert(2, 1 /* positive axis value*/);
        settings->setValue(section + "/Z", QVariant::fromValue(values));
        values.removeAt(2);
        values.replace(0, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        settings->setValue(section + "/L", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        settings->setValue(section + "/R", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_BUTTON_START);
        settings->setValue(section + "/Start", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        settings->setValue(section + "/DPadL", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        settings->setValue(section + "/DPadR", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_BUTTON_DPAD_UP);
        settings->setValue(section + "/DPadU", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        settings->setValue(section + "/DPadD", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_RIGHTX);
        values.insert(2, -1 /* negative axis value*/);
        settings->setValue(section + "/CLeft", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_RIGHTX);
        values.replace(2, 1 /* positive axis value*/);
        settings->setValue(section + "/CRight", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_RIGHTY);
        values.replace(2, 1 /* positive axis value*/);
        settings->setValue(section + "/CUp", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_RIGHTY);
        values.replace(2, -1 /* negative axis value*/);
        settings->setValue(section + "/CDown", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_LEFTX);
        values.replace(2, -1 /* negative axis value*/);
        settings->setValue(section + "/AxisLeft", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_LEFTX);
        values.replace(2, 1 /* positive axis value*/);
        settings->setValue(section + "/AxisRight", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_LEFTY);
        values.replace(2, 1 /* positive axis value*/);
        settings->setValue(section + "/AxisUp", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_LEFTY);
        values.replace(2, -1 /* negative axis value*/);
        settings->setValue(section + "/AxisDown", QVariant::fromValue(values));

        settings->setValue(section + "/Deadzone", 12.5);
    }

    l_PluginInit = 1;

    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginShutdown(void)
{
    if (!l_PluginInit)
        return M64ERR_NOT_INIT;

    l_PluginInit = 0;

    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion, int *APIVersion, const char **PluginNamePtr, int *Capabilities)
{
    /* set version info */
    if (PluginType != NULL)
        *PluginType = M64PLUGIN_INPUT;

    if (PluginVersion != NULL)
        *PluginVersion = QT_INPUT_PLUGIN_VERSION;

    if (APIVersion != NULL)
        *APIVersion = INPUT_PLUGIN_API_VERSION;

    if (PluginNamePtr != NULL)
        *PluginNamePtr = "Mupen64Plus Qt Input Plugin";

    if (Capabilities != NULL)
    {
        *Capabilities = 0;
    }

    return M64ERR_SUCCESS;
}

EXPORT void CALL ControllerCommand(int, unsigned char *)
{
}

EXPORT void CALL GetKeys( int Control, BUTTONS *Keys )
{
//    if (pads[Control] == NULL)
 //       return;

//    Keys->R_DPAD       = pads[Control]->buttonRight();
 //   Keys->L_DPAD       = pads[Control]->buttonLeft();
 //   Keys->D_DPAD       = pads[Control]->buttonDown();
  //  Keys->U_DPAD       = pads[Control]->buttonUp();
  //  Keys->START_BUTTON = pads[Control]->buttonStart();
  //  Keys->Z_TRIG       = pads[Control]->buttonL2();
  //  Keys->B_BUTTON     = pads[Control]->buttonX();
  //  Keys->A_BUTTON     = pads[Control]->buttonA();
  //  Keys->R_CBUTTON    = pads[Control]->axisRightX() > 0;
  //  Keys->L_CBUTTON    = pads[Control]->axisRightX() < 0;
  //  Keys->D_CBUTTON    = pads[Control]->axisRightY() > 0;
  //  Keys->U_CBUTTON    = pads[Control]->axisRightY() < 0;
  //  Keys->R_TRIG       = pads[Control]->buttonR1();
  //  Keys->L_TRIG       = pads[Control]->buttonL1();

//    Keys->X_AXIS       = pads[Control]->axisLeftX();
  //  Keys->Y_AXIS       = pads[Control]->axisLeftY();
}

EXPORT void CALL InitiateControllers(CONTROL_INFO ControlInfo)
{
    int i;

    // reset controllers
    memset( controller, 0, sizeof( SController ) * 4 );
    for ( i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        myKeyState[i] = 0;
    }
    // set our CONTROL struct pointers to the array that was passed in to this function from the core
    // this small struct tells the core whether each controller is plugged in, and what type of pak is connected
    for (i = 0; i < 4; i++)
        controller[i].control = ControlInfo.Controls + i;

    controller[0].control->Present = 1;
//    pads[0] = NULL;
 //   pads[1] = NULL;
  //  pads[2] = NULL;
   // pads[3] = NULL;
}

EXPORT void CALL ReadController(int, unsigned char *)
{
}

EXPORT int CALL RomOpen(void)
{
    return 1;
}

EXPORT void CALL RomClosed(void)
{

}

EXPORT void CALL SDL_KeyDown(int, int keysym)
{
    myKeyState[keysym] = 1;
}

EXPORT void CALL SDL_KeyUp(int, int keysym)
{
    myKeyState[keysym] = 0;
}

EXPORT void CALL Config_DoConfig()
{
    ConfigDialog config;
    config.exec();
}
