#pragma once

#include <QGroupBox>

class AddLine:public QGroupBox {
    Q_OBJECT

    public:
        AddLine(QWidget* pwgt, QString title);
    signals:
        void addSignal();
};