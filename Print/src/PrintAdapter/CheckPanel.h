#pragma once

#include <QFrame >
#include <QHash>
#include <QVector>
#include <QCheckBox>

class CheckPanel: public QFrame  {
    Q_OBJECT

    private:
        QHash<QString, QCheckBox*> m_checkboxes;

    public:
        CheckPanel(const QString &panelName, QHash<QString, QString> checkBoxNames, QWidget* pwgt = nullptr);

        bool getCheckBoxState(const QString &checkBoxName);
        void setCheckBoxState(const QString &checkBoxName, bool check);
        void setCheckBoxActive(const QString &checkBoxName, bool active);


};