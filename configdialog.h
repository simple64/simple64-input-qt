#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QSettings>
#include <QPushButton>
#include <SDL2/SDL.h>

extern QSettings* settings;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog();

private:
    QTabWidget *tabWidget;
};

class ControllerTab : public QWidget
{
    Q_OBJECT

public:
    ControllerTab(unsigned int controller);
};

class ProfileTab : public QWidget
{
    Q_OBJECT

public:
    ProfileTab();
};

class ProfileEditor : public QDialog
{
    Q_OBJECT

public:
    ProfileEditor(QString profile);
};

class CustomButton : public QPushButton
{
    Q_OBJECT

public:
    CustomButton(QString section, QString key);
    int type; //0 = Keyboard, 1 = Button, 2 = Axis
    int axisValue;
    SDL_GameControllerButton button;
    SDL_GameControllerAxis axis;
    SDL_Keycode key;
    QString item;
};


#endif
