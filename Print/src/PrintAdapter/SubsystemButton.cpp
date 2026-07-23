#include "SubsystemButton.h"
#include <QVariant>

SubsystemButton::SubsystemButton(
    const QString &buttonName,
    const QString &activePath, 
    const QString &inActivePath, 
    const QString &styleClass,
    const QString &subsystemName, 
    QVector<QString> tableList,
    QWidget* pwgt
): 
    QPushButton(buttonName, pwgt),
    m_activeIcon(activePath),       
    m_inActiveIcon(inActivePath) 
{
    setCheckable(true);
    setProperty("name", subsystemName);
    setProperty("tableList", QVariant::fromValue(tableList));
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