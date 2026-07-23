#pragma once

#include <QGroupBox>
#include <QPushButton>
#include <QEvent>

class AddLine:public QGroupBox {
    Q_OBJECT
    private:
        QPushButton* addButton;
    protected:
        bool eventFilter(QObject *watched, QEvent *event) override;

    public:
        AddLine(QWidget* pwgt, QString title);
    signals:
        void addSignal();
};