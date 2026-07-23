#include "CheckPanel.h"
#include <QVBoxLayout>
#include <QLabel>

CheckPanel::CheckPanel(
    const QString &panelName,
    QHash<QString, QString> checkBoxNames, 
    QWidget* pwgt
): QFrame (pwgt) {
    setObjectName("SettingPanel");
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(14, 10, 14, 12);

    QLabel* label = new QLabel(panelName, this);
    layout->addWidget(label);

    QHBoxLayout* checkBoxesLayout = new QHBoxLayout(this);
    checkBoxesLayout->setContentsMargins(0, 0, 0, 0);
    checkBoxesLayout->setSpacing(20);

    for (auto element = checkBoxNames.constBegin(); element != checkBoxNames.constEnd(); element++) {
        QCheckBox* checkBox = new QCheckBox(element.key(), this);
        m_checkboxes[element.value()] = checkBox;
        checkBoxesLayout->addWidget(checkBox);
    }

    layout->addLayout(checkBoxesLayout);
}

bool CheckPanel::getCheckBoxState(const QString &checkBoxName) {
    QCheckBox* checkBox = m_checkboxes.value(checkBoxName, nullptr);
    if (checkBox == nullptr) {
        return false;
    }
    return checkBox->isChecked();
}

void CheckPanel::setCheckBoxState(const QString &checkBoxName, bool check) {
    QCheckBox* checkBox = m_checkboxes.value(checkBoxName, nullptr);
    if (checkBox == nullptr) {
        return;
    }
    checkBox->setChecked(check);
}

void CheckPanel::setCheckBoxActive(const QString &checkBoxName, bool active) {
    QCheckBox* checkBox = m_checkboxes.value(checkBoxName, nullptr);
    if (checkBox == nullptr) {
        return;
    }
    
    if (active) {
        checkBox->setEnabled(active);
    } else {
        checkBox->setDisabled(!active);
    }
}