#include "SubsystemButton.h"
#include <QVariant>
#include <QVector>

SubsystemButton::SubsystemButton(
    const QString &buttonName,
    const QString &activePath, 
    const QString &inActivePath, 
    const QString &styleClass,
    const QString &subsystemName, 
    QWidget* pwgt
): 
    QPushButton(buttonName, pwgt),
    m_activeIcon(activePath),       
    m_inActiveIcon(inActivePath),
    subsystem(subsystemName)
{
    setCheckable(true);
    setObjectName(styleClass);
    setIcon(m_inActiveIcon);
}

void SubsystemButton::nextCheckState() {
    QPushButton::nextCheckState();
    
    if (isChecked()) {
        setIcon(m_activeIcon);
    } else {
        setIcon(m_inActiveIcon);
    }
}