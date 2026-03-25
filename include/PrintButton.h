#pragma once

#include <QtWidgets>

class PrintButton: public QPushButton {
    Q_OBJECT

    public:
        PrintButton(QWidget* pwgt = nullptr);

    private slots:
        void requestDialogSlot();
};