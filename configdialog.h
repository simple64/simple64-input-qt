#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QSettings>
#include <QPushButton>
#include <QComboBox>
#include <SDL2/SDL.h>

extern QSettings* settings;
extern QSettings* controllerSettings;

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
private:
    QComboBox *profileSelect;
    QComboBox *gamepadSelect;
    QComboBox *pakSelect;
};

class ProfileTab : public QWidget
{
    Q_OBJECT

public:
    ProfileTab(ControllerTab **_controllerTabs);
private:
    void setComboBox(QComboBox* box);
    ControllerTab *controllerTabs[4];
};

class CustomButton : public QPushButton
{
    Q_OBJECT

public:
    CustomButton(QString section, QString setting, QWidget* parent);
    int type; //0 = Keyboard, 1 = Button, 2 = Axis
    int axisValue;
    SDL_GameControllerButton button;
    SDL_GameControllerAxis axis;
    SDL_Scancode key;
    QString item;
    QString origText;
};

class ProfileEditor : public QDialog
{
    Q_OBJECT

public:
    ProfileEditor(QString profile);
    void acceptInput(CustomButton* button);
protected:
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
private:
    QList<CustomButton*> buttonList;
    CustomButton* activeButton;
    int buttonTimer;
    int timer;
};

#endif
