/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-input-qt - main.cpp                                       *
 *   Mupen64Plus homepage: https://mupen64plus.org/                        *
 *   Copyright (C) 2008-2011 Richard Goedeken                              *
 *   Copyright (C) 2008 Tillin9                                            *
 *   Copyright (C) 2002 Blight                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define M64P_PLUGIN_PROTOTYPES 1
#include "m64p_common.h"
#include "m64p_types.h"
#include "m64p_config.h"
#include "m64p_plugin.h"
#include "main.h"
#include "configdialog.h"
#include "osal/osal_dynamiclib.h"

#include <QDir>

#define QT_INPUT_PLUGIN_VERSION 0x020500
#define INPUT_PLUGIN_API_VERSION 0x020100
static int l_PluginInit = 0;
static unsigned char myKeyState[SDL_NUM_SCANCODES];
QSettings* settings;
QSettings* controllerSettings;
SController controller[4];   // 4 controllers

Q_DECLARE_METATYPE(QList<int>)

EXPORT m64p_error CALL PluginStartup(m64p_dynlib_handle CoreHandle, void *, void (*)(void *, int, const char *))
{
    if (l_PluginInit)
        return M64ERR_ALREADY_INIT;

    ptr_ConfigGetUserConfigPath ConfigGetUserConfigPath = (ptr_ConfigGetUserConfigPath) osal_dynlib_getproc(CoreHandle, "ConfigGetUserConfigPath");
    QDir ini_path(ConfigGetUserConfigPath());
    settings = new QSettings(ini_path.absoluteFilePath("input-profiles.ini"), QSettings::IniFormat);
    controllerSettings = new QSettings(ini_path.absoluteFilePath("input-settings.ini"), QSettings::IniFormat);

    QString section;
    for (int i = 1; i < 5; ++i) {
        section = "Controller" + QString::number(i);
        if (!controllerSettings->childGroups().contains(section)) {
            controllerSettings->setValue(section + "/Profile", "Auto");
            controllerSettings->setValue(section + "/Gamepad", "Auto");
            controllerSettings->setValue(section + "/Pak", "Memory");
        }
    }

    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");

    QList<int> values;
    section = "Auto-Keyboard";
    values.insert(0, 0/*blank value*/);
    values.insert(1, 0/*Keyboard*/);
    if (!settings->childGroups().contains(section)) {
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
    values.replace(1, 1/*Button*/);
    if (!settings->childGroups().contains(section)) {
        values.replace(0, SDL_CONTROLLER_BUTTON_A);
        settings->setValue(section + "/A", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_BUTTON_X);
        settings->setValue(section + "/B", QVariant::fromValue(values));
        values.replace(0, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        values.replace(1, 2/*Axis*/);
        values.insert(2, 1 /* positive axis value*/);
        settings->setValue(section + "/Z", QVariant::fromValue(values));
        values.removeAt(2);
        values.replace(1, 1/*Button*/);
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
        values.replace(1, 2/*Axis*/);
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

    if (!SDL_WasInit(SDL_INIT_JOYSTICK))
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    l_PluginInit = 1;

    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginShutdown(void)
{
    if (!l_PluginInit)
        return M64ERR_NOT_INIT;

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
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
}

EXPORT void CALL InitiateControllers(CONTROL_INFO ControlInfo)
{
    int i, j;
    // reset controllers
    memset( controller, 0, sizeof( SController ) * 4 );
    for (i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        myKeyState[i] = 0;
    }
    // set our CONTROL struct pointers to the array that was passed in to this function from the core
    // this small struct tells the core whether each controller is plugged in, and what type of pak is connected
    QString pak;
    QString gamepad;
    int controller_index;
    QString gamepad_name;
    int auto_index = 0;
    int used_index[4] = {-1, -1, -1, -1};
    for (i = 0; i < 4; i++) {
        controller[i].control = ControlInfo.Controls + i;
        controller[i].control->RawData = 0;
        pak = controllerSettings->value("Controller" + QString::number(i + 1) + "/Pak").toString();
        gamepad = controllerSettings->value("Controller" + QString::number(i + 1) + "/Gamepad").toString();
        if (gamepad == "Keyboard")
            controller[i].control->Present = 1;
        else if (gamepad == "None")
            controller[i].control->Present = 0;
        else if (gamepad == "Auto") {
            for (j = 0; j < 4; ++j) {
                if (auto_index == used_index[j]) {
                    ++auto_index;
                    j = 0;
                }
            }
            if (SDL_IsGameController(auto_index)) {
                controller[i].gamepad = SDL_GameControllerOpen(auto_index);
                ++auto_index;
                controller[i].control->Present = 1;
            }
            if (i == 0) controller[i].control->Present = 1; //Player 1
        }
        else /*specific gamepad selected*/ {
            controller_index = gamepad.split(":")[0].toInt();
            gamepad_name = gamepad.split(":")[1];
            if (SDL_IsGameController(controller_index)) {
                if (gamepad_name == SDL_GameControllerNameForIndex(controller_index)) {
                    controller[i].gamepad = SDL_GameControllerOpen(controller_index);
                    used_index[i] = controller_index;
                    if (controller[i].gamepad != NULL)
                        controller[i].control->Present = 1;
                }
            }
        }

        controller[i].profile = controllerSettings->value("Controller" + QString::number(i + 1) + "/Profile").toString();
        if (controller[i].profile == "Auto") {
            if (controller[i].gamepad != NULL)
                controller[i].profile = "Auto-Gamepad";
            else
                controller[i].profile = "Auto-Keyboard";
        }

        if (pak == "Transfer")
            controller[i].control->Plugin = PLUGIN_TRANSFER_PAK;
        else if (pak == "Rumble")
            controller[i].control->Plugin = PLUGIN_RUMBLE_PAK;
        else if (pak == "None")
            controller[i].control->Plugin = PLUGIN_NONE;
        else
            controller[i].control->Plugin = PLUGIN_MEMPAK;
    }
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
