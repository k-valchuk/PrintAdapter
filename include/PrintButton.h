#pragma once

#include <QtWidgets>
#include "ServerRequester.h"

class PrintButton: public QPushButton {
    Q_OBJECT

    private:
        ServerRequester* server_requester;

    public:
        PrintButton(QWidget* pwgt = nullptr);

    private slots:
        void requestDialogSlot();
};