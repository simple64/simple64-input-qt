#include "configdialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

ControllerTab::ControllerTab(unsigned int controller)
{
    QGridLayout *layout = new QGridLayout;
    QLabel *profileLabel = new QLabel("Profile");
    layout->addWidget(profileLabel, 0, 0);
    QComboBox *profileSelect = new QComboBox;
    layout->addWidget(profileSelect, 0, 1);
    QLabel *gamepadLabel = new QLabel("Gamepad");
    layout->addWidget(gamepadLabel, 1, 0);
    QComboBox *gamepadSelect = new QComboBox;
    layout->addWidget(gamepadSelect, 1, 1);
    QLabel *pakLabel = new QLabel("Pak");
    layout->addWidget(pakLabel, 2, 0);
    QComboBox *pakSelect = new QComboBox;
    layout->addWidget(pakSelect, 2, 1);
    setLayout(layout);
}

ProfileTab::ProfileTab()
{
    QGridLayout *layout = new QGridLayout;
    QComboBox *profileSelect = new QComboBox;
    QPushButton *buttonNew = new QPushButton("New Profile");
    QPushButton *buttonEdit = new QPushButton("Edit Profile");
    QPushButton *buttonDelete = new QPushButton("Delete Profile");
    layout->addWidget(profileSelect, 1, 0);
    layout->addWidget(buttonNew, 0, 1);
    layout->addWidget(buttonEdit, 1, 1);
    layout->addWidget(buttonDelete, 1, 2);
    setLayout(layout);
}


ConfigDialog::ConfigDialog()
{
    unsigned int i;

    tabWidget = new QTabWidget;
    tabWidget->setUsesScrollButtons(0);
    for (i = 1; i < 5; ++i)
        tabWidget->addTab(new ControllerTab(i), "Controller " + QString::number(i));

    tabWidget->addTab(new ProfileTab(), tr("Manage Profiles"));
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Controller Configuration"));
}
