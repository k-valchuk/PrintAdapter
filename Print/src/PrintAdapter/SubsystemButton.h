#pragma once

#include <QPushButton>
#include <QIcon>
#include <QVector>

class SubsystemButton: public QPushButton {
    Q_OBJECT

    private:
        QIcon m_activeIcon;
        QIcon m_inActiveIcon;
    
    protected:
        void nextCheckState() override;
        
    public:
        SubsystemButton(
            const QString &buttonName,
            const QString &activePath, 
            const QString &inActivePath, 
            const QString &styleClass, 
            const QString &subsystemName, 
            QVector<QString> tableList,
            QWidget* pwgt
        );
};
