#pragma once

#include <QtWidgets>
#include "ServerRequester.h"

class PrintButton: public QPushButton {
    Q_OBJECT

    private:
        ServerRequester* server_requester;

    public:
        PrintButton(QWidget* pwgt = nullptr);
        void setExportJson(QJsonDocument json_doc);
        ~PrintButton();

    private slots:
        void requestDialogSlot();
};