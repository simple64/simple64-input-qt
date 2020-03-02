#include "configdialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <SDL2/SDL.h>

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
    connect(buttonNew, &QPushButton::released, [=]() {
        ProfileEditor editor("");
        editor.exec();
    });
    QPushButton *buttonEdit = new QPushButton("Edit Profile");
    connect(buttonEdit, &QPushButton::released, [=]() {
        if (!profileSelect->currentText().isEmpty()) {
            ProfileEditor editor(profileSelect->currentText());
            editor.exec();
        }
    });

    QPushButton *buttonDelete = new QPushButton("Delete Profile");
    connect(buttonDelete, &QPushButton::released, [=]() {
        if (!profileSelect->currentText().isEmpty()) {
            profileSelect->removeItem(profileSelect->currentIndex());
            settings->remove(profileSelect->currentText());
        }
    });

    layout->addWidget(profileSelect, 1, 0);
    layout->addWidget(buttonNew, 0, 1);
    layout->addWidget(buttonEdit, 1, 1);
    layout->addWidget(buttonDelete, 1, 2);
    setLayout(layout);
}

CustomButton::CustomButton(QString section, QString key)
{
    QList<int> value = settings->value(section + "/" + key).value<QList<int> >();
    if (value.at(1) == 0)
        this->setText(SDL_GetKeyName(value.at(0)));
    else if (value.size() == 2)
        this->setText(SDL_GameControllerGetStringForButton((SDL_GameControllerButton)value.at(0)));
    else
        this->setText(SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)value.at(0)));
}

ProfileEditor::ProfileEditor(QString profile)
{
    QString section;
    if (profile.isEmpty())
        section = "Auto-Keyboard";
    else
        section = profile;

    QGridLayout *layout = new QGridLayout;
    QLabel *profileNameLabel = new QLabel("Profile Name");
    QLineEdit *profileName = new QLineEdit;
    profileName->setText(profile);
    layout->addWidget(profileNameLabel, 0, 3);
    layout->addWidget(profileName, 0, 4);

    QFrame* lineH = new QFrame();
    lineH->setFrameShape(QFrame::HLine);
    lineH->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineH, 1, 0, 1, 8);

    QLabel *buttonLabelA = new QLabel("A");
    buttonLabelA->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushA = new CustomButton(section, "A");
    layout->addWidget(buttonLabelA, 2, 0);
    layout->addWidget(buttonPushA, 2, 1);

    QLabel *buttonLabelB = new QLabel("B");
    buttonLabelB->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushB = new CustomButton(section, "B");
    layout->addWidget(buttonLabelB, 3, 0);
    layout->addWidget(buttonPushB, 3, 1);

    QLabel *buttonLabelZ = new QLabel("Z");
    buttonLabelZ->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushZ = new CustomButton(section, "Z");
    layout->addWidget(buttonLabelZ, 4, 0);
    layout->addWidget(buttonPushZ, 4, 1);

    QLabel *buttonLabelStart = new QLabel("Start");
    buttonLabelStart->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStart = new CustomButton(section, "Start");
    layout->addWidget(buttonLabelStart, 5, 0);
    layout->addWidget(buttonPushStart, 5, 1);

    QLabel *buttonLabelLTrigger = new QLabel("Left Trigger");
    buttonLabelLTrigger->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushLTrigger = new CustomButton(section, "L");
    layout->addWidget(buttonLabelLTrigger, 6, 0);
    layout->addWidget(buttonPushLTrigger, 6, 1);

    QLabel *buttonLabelRTrigger = new QLabel("Right Trigger");
    buttonLabelRTrigger->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushRTrigger = new CustomButton(section, "R");
    layout->addWidget(buttonLabelRTrigger, 7, 0);
    layout->addWidget(buttonPushRTrigger, 7, 1);

    QFrame* lineV = new QFrame();
    lineV->setFrameShape(QFrame::VLine);
    lineV->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineV, 2, 2, 6, 1);

    QLabel *buttonLabelDPadL = new QLabel("DPad Left");
    buttonLabelDPadL->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushDPadL = new CustomButton(section, "DPadL");
    layout->addWidget(buttonLabelDPadL, 2, 3);
    layout->addWidget(buttonPushDPadL, 2, 4);

    QLabel *buttonLabelDPadR = new QLabel("DPad Right");
    buttonLabelDPadR->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushDPadR = new CustomButton(section, "DPadR");
    layout->addWidget(buttonLabelDPadR, 3, 3);
    layout->addWidget(buttonPushDPadR, 3, 4);

    QLabel *buttonLabelDPadU = new QLabel("DPad Up");
    buttonLabelDPadU->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushDPadU = new CustomButton(section, "DPadU");
    layout->addWidget(buttonLabelDPadU, 4, 3);
    layout->addWidget(buttonPushDPadU, 4, 4);

    QLabel *buttonLabelDPadD = new QLabel("DPad Down");
    buttonLabelDPadD->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushDPadD = new CustomButton(section, "DPadD");
    layout->addWidget(buttonLabelDPadD, 5, 3);
    layout->addWidget(buttonPushDPadD, 5, 4);

    QFrame* lineV2 = new QFrame();
    lineV2->setFrameShape(QFrame::VLine);
    lineV2->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineV2, 2, 5, 6, 1);

    QLabel *buttonLabelCL = new QLabel("C Left");
    buttonLabelCL->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushCL = new CustomButton(section, "CLeft");
    layout->addWidget(buttonLabelCL, 2, 6);
    layout->addWidget(buttonPushCL, 2, 7);

    QLabel *buttonLabelCR = new QLabel("C Right");
    buttonLabelCR->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushCR = new CustomButton(section, "CRight");
    layout->addWidget(buttonLabelCR, 3, 6);
    layout->addWidget(buttonPushCR, 3, 7);

    QLabel *buttonLabelCU = new QLabel("C Up");
    buttonLabelCU->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushCU = new CustomButton(section, "CUp");
    layout->addWidget(buttonLabelCU, 4, 6);
    layout->addWidget(buttonPushCU, 4, 7);

    QLabel *buttonLabelCD = new QLabel("C Down");
    buttonLabelCD->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushCD = new CustomButton(section, "CDown");
    layout->addWidget(buttonLabelCD, 5, 6);
    layout->addWidget(buttonPushCD, 5, 7);


    QLabel *buttonLabelStickL = new QLabel("Control Stick Left");
    buttonLabelStickL->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStickL = new CustomButton(section, "AxisLeft");
    layout->addWidget(buttonLabelStickL, 6, 3);
    layout->addWidget(buttonPushStickL, 6, 4);

    QLabel *buttonLabelStickR = new QLabel("Control Stick Right");
    buttonLabelStickR->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStickR = new CustomButton(section, "AxisRight");
    layout->addWidget(buttonLabelStickR, 7, 3);
    layout->addWidget(buttonPushStickR, 7, 4);

    QLabel *buttonLabelStickU = new QLabel("Control Stick Up");
    buttonLabelStickU->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStickU = new CustomButton(section, "AxisUp");;
    layout->addWidget(buttonLabelStickU, 6, 6);
    layout->addWidget(buttonPushStickU, 6, 7);

    QLabel *buttonLabelStickD = new QLabel("Control Stick Down");
    buttonLabelStickD->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStickD = new CustomButton(section, "AxisDown");;
    layout->addWidget(buttonLabelStickD, 7, 6);
    layout->addWidget(buttonPushStickD, 7, 7);

    QFrame* lineH2 = new QFrame();
    lineH2->setFrameShape(QFrame::HLine);
    lineH2->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineH2, 8, 0, 1, 8);

    QLabel *buttonLabelDeadzone = new QLabel("Deadzone");
    buttonLabelDeadzone->setAlignment(Qt::AlignCenter);
    QLabel *buttonLabelDeadzoneValue = new QLabel;
    float deadzoneValue = settings->value(section + "/Deadzone").toFloat();
    buttonLabelDeadzoneValue->setText(QString::number(deadzoneValue) + "%");
    buttonLabelDeadzoneValue->setAlignment(Qt::AlignCenter);
    QSlider *sliderDeadzone = new QSlider(Qt::Horizontal);
    sliderDeadzone->setMinimum(0);
    sliderDeadzone->setMaximum(250);
    sliderDeadzone->setTickPosition(QSlider::TicksBothSides);
    sliderDeadzone->setTickInterval(5);
    sliderDeadzone->setSliderPosition((int)(deadzoneValue * 10.0));
    connect(sliderDeadzone, &QSlider::valueChanged, [=](int value) {
        float percent = value / 10.0;
        buttonLabelDeadzoneValue->setText(QString::number(percent, 'f', 1) + "%");
    });

    layout->addWidget(buttonLabelDeadzone, 9, 0);
    layout->addWidget(buttonLabelDeadzoneValue, 9, 1);
    layout->addWidget(sliderDeadzone, 9, 2, 1, 6);

    QFrame* lineH3 = new QFrame();
    lineH3->setFrameShape(QFrame::HLine);
    lineH3->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineH3, 10, 0, 1, 8);

    QPushButton *buttonPushSave = new QPushButton("Save and Close");
    connect(buttonPushSave, &QPushButton::released, [=]() {
        const QString saveSection = profileName->text();
        this->done(1);
    });
    layout->addWidget(buttonPushSave, 11, 0, 1, 2);
    QPushButton *buttonPushClose = new QPushButton("Close Without Saving");
    connect(buttonPushClose, &QPushButton::released, [=]() {
        this->done(1);
    });
    layout->addWidget(buttonPushClose, 11, 6, 1, 2);

    setLayout(layout);
    setWindowTitle(tr("Profile Editor"));
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
