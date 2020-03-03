#include "configdialog.h"
#include "qt2sdl2.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QKeyEvent>

ControllerTab::ControllerTab(unsigned int controller)
{
    QGridLayout *layout = new QGridLayout;
    QLabel *profileLabel = new QLabel("Profile");
    layout->addWidget(profileLabel, 0, 0);

    profileSelect = new QComboBox;
    profileSelect->addItems(settings->childGroups());
    profileSelect->removeItem(profileSelect->findText("Auto-Gamepad"));
    profileSelect->removeItem(profileSelect->findText("Auto-Keyboard"));
    profileSelect->insertItem(0, "Auto");
    profileSelect->setCurrentText(controllerSettings->value("Controller" + QString::number(controller) + "/Profile").toString());
    connect(profileSelect, &QComboBox::currentTextChanged, [=](QString text) {
        controllerSettings->setValue("Controller" + QString::number(controller) + "/Profile", text);
    });
    layout->addWidget(profileSelect, 0, 1);

    QLabel *gamepadLabel = new QLabel("Gamepad");
    layout->addWidget(gamepadLabel, 1, 0);

    gamepadSelect = new QComboBox;
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        gamepadSelect->addItem(QString::number(i) + ":" + SDL_GameControllerNameForIndex(i));
    gamepadSelect->insertItem(0, "Auto");
    gamepadSelect->addItem("Keyboard");
    gamepadSelect->setCurrentText(controllerSettings->value("Controller" + QString::number(controller) + "/Gamepad").toString());
    connect(gamepadSelect, &QComboBox::currentTextChanged, [=](QString text) {
        controllerSettings->setValue("Controller" + QString::number(controller) + "/Gamepad", text);
    });
    layout->addWidget(gamepadSelect, 1, 1);

    QLabel *pakLabel = new QLabel("Pak");
    layout->addWidget(pakLabel, 2, 0);

    pakSelect = new QComboBox;
    pakSelect->addItem("Memory");
    pakSelect->addItem("Rumble");
    pakSelect->addItem("Transfer");
    pakSelect->addItem("None");
    pakSelect->setCurrentText(controllerSettings->value("Controller" + QString::number(controller) + "/Pak").toString());
    connect(pakSelect, &QComboBox::currentTextChanged, [=](QString text) {
        controllerSettings->setValue("Controller" + QString::number(controller) + "/Pak", text);
    });
    layout->addWidget(pakSelect, 2, 1);

    setLayout(layout);
}

void ProfileTab::setComboBox(QComboBox* box, ControllerTab **_controllerTabs)
{
    for (int i = 1; i < 5; ++i) {
        QString current = controllerSettings->value("Controller" + QString::number(i) + "/Profile").toString();
        _controllerTabs[i-1]->profileSelect->clear();
        _controllerTabs[i-1]->profileSelect->addItems(settings->childGroups());
        _controllerTabs[i-1]->profileSelect->removeItem(_controllerTabs[i-1]->profileSelect->findText("Auto-Gamepad"));
        _controllerTabs[i-1]->profileSelect->removeItem(_controllerTabs[i-1]->profileSelect->findText("Auto-Keyboard"));
        _controllerTabs[i-1]->profileSelect->insertItem(0, "Auto");
        _controllerTabs[i-1]->profileSelect->setCurrentText(current);
    }
    box->clear();
    box->addItems(settings->childGroups());
    box->removeItem(box->findText("Auto-Gamepad"));
    box->removeItem(box->findText("Auto-Keyboard"));
}

ProfileTab::ProfileTab(ControllerTab **_controllerTabs)
{
    QGridLayout *layout = new QGridLayout;
    QComboBox *profileSelect = new QComboBox;
    setComboBox(profileSelect, _controllerTabs);
    QPushButton *buttonNew = new QPushButton("New Profile");
    connect(buttonNew, &QPushButton::released, [=]() {
        ProfileEditor editor("");
        editor.exec();
        setComboBox(profileSelect, _controllerTabs);
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
            settings->remove(profileSelect->currentText());
            setComboBox(profileSelect, _controllerTabs);
        }
    });

    layout->addWidget(profileSelect, 1, 0);
    layout->addWidget(buttonNew, 0, 1);
    layout->addWidget(buttonEdit, 1, 1);
    layout->addWidget(buttonDelete, 1, 2);
    setLayout(layout);
}

CustomButton::CustomButton(QString section, QString setting, QWidget* parent)
{
    ProfileEditor* editor = (ProfileEditor*) parent;
    item = setting;
    QList<int> value = settings->value(section + "/" + item).value<QList<int> >();
    if (value.at(1) == 0) {
        type = 0;
        key = (SDL_Scancode)value.at(0);
        this->setText(SDL_GetScancodeName(key));
    }
    else if (value.size() == 2) {
        type = 1;
        button = (SDL_GameControllerButton)value.at(0);
        this->setText(SDL_GameControllerGetStringForButton((SDL_GameControllerButton)value.at(0)));
    }
    else {
        type = 2;
        axis = (SDL_GameControllerAxis)value.at(0);
        axisValue = value.at(2);
        this->setText(SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)value.at(0)));
    }
    connect(this, &QPushButton::released, [=]{
        editor->acceptInput(this);
    });
}

void ProfileEditor::keyReleaseEvent(QKeyEvent *event)
{
    int modValue = QT2SDL2MOD(event->modifiers());
    int keyValue = QT2SDL2(event->key());
    SDL_Scancode value = (SDL_Scancode)((modValue << 16) + keyValue);
    if (activeButton != nullptr) {
        killTimer(timer);
        activeButton->type = 0;
        activeButton->key = value;
        activeButton->setText(SDL_GetScancodeName(activeButton->key));
        activeButton = nullptr;
        for (int i = 0; i < buttonList.size(); ++i) {
            buttonList.at(i)->setDisabled(0);
        }
    }
}

void ProfileEditor::timerEvent(QTimerEvent *)
{
    if (buttonTimer == 1) {
        killTimer(timer);
        activeButton->setText(activeButton->origText);
        activeButton = nullptr;
        for (int i = 0; i < buttonList.size(); ++i) {
            buttonList.at(i)->setDisabled(0);
        }
        return;
    }
    --buttonTimer;
    activeButton->setText(QString::number(buttonTimer));
}

void ProfileEditor::acceptInput(CustomButton* button)
{
    activeButton = button;
    for (int i = 0; i < buttonList.size(); ++i) {
        buttonList.at(i)->setDisabled(1);
    }
    buttonTimer = 5;
    activeButton->origText = activeButton->text();
    activeButton->setText(QString::number(buttonTimer));
    timer = startTimer(1000);
}

ProfileEditor::ProfileEditor(QString profile)
{
    activeButton = nullptr;
    QString section;
    QLineEdit *profileName = new QLineEdit;
    if (profile.isEmpty()) {
        section = "Auto-Keyboard";
        profileName->setDisabled(0);
    }
    else {
        section = profile;
        profileName->setDisabled(1);
    }

    QGridLayout *layout = new QGridLayout;
    QLabel *profileNameLabel = new QLabel("Profile Name");
    profileName->setText(profile);
    layout->addWidget(profileNameLabel, 0, 3);
    layout->addWidget(profileName, 0, 4);

    QFrame* lineH = new QFrame();
    lineH->setFrameShape(QFrame::HLine);
    lineH->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineH, 1, 0, 1, 8);

    QLabel *buttonLabelA = new QLabel("A");
    buttonLabelA->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushA = new CustomButton(section, "A", this);
    buttonList.append(buttonPushA);
    layout->addWidget(buttonLabelA, 2, 0);
    layout->addWidget(buttonPushA, 2, 1);

    QLabel *buttonLabelB = new QLabel("B");
    buttonLabelB->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushB = new CustomButton(section, "B", this);
    buttonList.append(buttonPushB);
    layout->addWidget(buttonLabelB, 3, 0);
    layout->addWidget(buttonPushB, 3, 1);

    QLabel *buttonLabelZ = new QLabel("Z");
    buttonLabelZ->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushZ = new CustomButton(section, "Z", this);
    buttonList.append(buttonPushZ);
    layout->addWidget(buttonLabelZ, 4, 0);
    layout->addWidget(buttonPushZ, 4, 1);

    QLabel *buttonLabelStart = new QLabel("Start");
    buttonLabelStart->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStart = new CustomButton(section, "Start", this);
    buttonList.append(buttonPushStart);
    layout->addWidget(buttonLabelStart, 5, 0);
    layout->addWidget(buttonPushStart, 5, 1);

    QLabel *buttonLabelLTrigger = new QLabel("Left Trigger");
    buttonLabelLTrigger->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushLTrigger = new CustomButton(section, "L", this);
    buttonList.append(buttonPushLTrigger);
    layout->addWidget(buttonLabelLTrigger, 6, 0);
    layout->addWidget(buttonPushLTrigger, 6, 1);

    QLabel *buttonLabelRTrigger = new QLabel("Right Trigger");
    buttonLabelRTrigger->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushRTrigger = new CustomButton(section, "R", this);
    buttonList.append(buttonPushRTrigger);
    layout->addWidget(buttonLabelRTrigger, 7, 0);
    layout->addWidget(buttonPushRTrigger, 7, 1);

    QFrame* lineV = new QFrame();
    lineV->setFrameShape(QFrame::VLine);
    lineV->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineV, 2, 2, 6, 1);

    QLabel *buttonLabelDPadL = new QLabel("DPad Left");
    buttonLabelDPadL->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushDPadL = new CustomButton(section, "DPadL", this);
    buttonList.append(buttonPushDPadL);
    layout->addWidget(buttonLabelDPadL, 2, 3);
    layout->addWidget(buttonPushDPadL, 2, 4);

    QLabel *buttonLabelDPadR = new QLabel("DPad Right");
    buttonLabelDPadR->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushDPadR = new CustomButton(section, "DPadR", this);
    buttonList.append(buttonPushDPadR);
    layout->addWidget(buttonLabelDPadR, 3, 3);
    layout->addWidget(buttonPushDPadR, 3, 4);

    QLabel *buttonLabelDPadU = new QLabel("DPad Up");
    buttonLabelDPadU->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushDPadU = new CustomButton(section, "DPadU", this);
    buttonList.append(buttonPushDPadU);
    layout->addWidget(buttonLabelDPadU, 4, 3);
    layout->addWidget(buttonPushDPadU, 4, 4);

    QLabel *buttonLabelDPadD = new QLabel("DPad Down");
    buttonLabelDPadD->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushDPadD = new CustomButton(section, "DPadD", this);
    buttonList.append(buttonPushDPadD);
    layout->addWidget(buttonLabelDPadD, 5, 3);
    layout->addWidget(buttonPushDPadD, 5, 4);

    QFrame* lineV2 = new QFrame();
    lineV2->setFrameShape(QFrame::VLine);
    lineV2->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineV2, 2, 5, 6, 1);

    QLabel *buttonLabelCL = new QLabel("C Left");
    buttonLabelCL->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushCL = new CustomButton(section, "CLeft", this);
    buttonList.append(buttonPushCL);
    layout->addWidget(buttonLabelCL, 2, 6);
    layout->addWidget(buttonPushCL, 2, 7);

    QLabel *buttonLabelCR = new QLabel("C Right");
    buttonLabelCR->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushCR = new CustomButton(section, "CRight", this);
    buttonList.append(buttonPushCR);
    layout->addWidget(buttonLabelCR, 3, 6);
    layout->addWidget(buttonPushCR, 3, 7);

    QLabel *buttonLabelCU = new QLabel("C Up");
    buttonLabelCU->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushCU = new CustomButton(section, "CUp", this);
    buttonList.append(buttonPushCU);
    layout->addWidget(buttonLabelCU, 4, 6);
    layout->addWidget(buttonPushCU, 4, 7);

    QLabel *buttonLabelCD = new QLabel("C Down");
    buttonLabelCD->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushCD = new CustomButton(section, "CDown", this);
    buttonList.append(buttonPushCD);
    layout->addWidget(buttonLabelCD, 5, 6);
    layout->addWidget(buttonPushCD, 5, 7);


    QLabel *buttonLabelStickL = new QLabel("Control Stick Left");
    buttonLabelStickL->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStickL = new CustomButton(section, "AxisLeft", this);
    buttonList.append(buttonPushStickL);
    layout->addWidget(buttonLabelStickL, 6, 3);
    layout->addWidget(buttonPushStickL, 6, 4);

    QLabel *buttonLabelStickR = new QLabel("Control Stick Right");
    buttonLabelStickR->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStickR = new CustomButton(section, "AxisRight", this);
    buttonList.append(buttonPushStickR);
    layout->addWidget(buttonLabelStickR, 7, 3);
    layout->addWidget(buttonPushStickR, 7, 4);

    QLabel *buttonLabelStickU = new QLabel("Control Stick Up");
    buttonLabelStickU->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStickU = new CustomButton(section, "AxisUp", this);
    buttonList.append(buttonPushStickU);
    layout->addWidget(buttonLabelStickU, 6, 6);
    layout->addWidget(buttonPushStickU, 6, 7);

    QLabel *buttonLabelStickD = new QLabel("Control Stick Down");
    buttonLabelStickD->setAlignment(Qt::AlignCenter);
    CustomButton *buttonPushStickD = new CustomButton(section, "AxisDown", this);
    buttonList.append(buttonPushStickD);
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
        if (!saveSection.startsWith("Auto-") && !saveSection.isEmpty() && (!settings->contains(saveSection + "/A") || !profileName->isEnabled())) {
            QList<int> value;
            for (int i = 0; i < buttonList.size(); ++i) {
                value.clear();
                if (buttonList.at(i)->type == 0) {
                    value.insert(0, buttonList.at(i)->key);
                    value.insert(1, 0);
                }
                else if (buttonList.at(i)->type == 1) {
                    value.insert(0, buttonList.at(i)->button);
                    value.insert(1, 1);
                }
                else if (buttonList.at(i)->type == 2) {
                    value.insert(0, buttonList.at(i)->axis);
                    value.insert(1, 1);
                    value.insert(2, buttonList.at(i)->axisValue);
                }
                settings->setValue(saveSection + "/" + buttonList.at(i)->item, QVariant::fromValue(value));
            }
            float percent = sliderDeadzone->value() / 10.0;
            settings->setValue(saveSection + "/Deadzone", percent);
            this->done(1);
        }
        else {
            QMessageBox msgBox;
            msgBox.setText("Invalid profile name.");
            msgBox.exec();
        }
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
    for (i = 1; i < 5; ++i) {
        controllerTabs[i-1] = new ControllerTab(i);
        tabWidget->addTab(controllerTabs[i-1], "Controller " + QString::number(i));
    }

    tabWidget->addTab(new ProfileTab(controllerTabs), tr("Manage Profiles"));
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Controller Configuration"));
}
