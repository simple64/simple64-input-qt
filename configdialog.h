#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QSettings>

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

#endif
