#include "configdialog.h"

ControllerTab::ControllerTab(unsigned int controller)
{
}

ProfileTab::ProfileTab()
{
}


ConfigDialog::ConfigDialog()
{
    unsigned int i;
    tabWidget = new QTabWidget;
    for (i = 1; i < 5; ++i)
        tabWidget->addTab(new ControllerTab(i), "Controller " + QString::number(i));

    tabWidget->addTab(new ProfileTab(), tr("Manage Profiles"));
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Controller Configuration"));
}
