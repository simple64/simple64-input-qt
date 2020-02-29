#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QTabWidget>

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


#endif
