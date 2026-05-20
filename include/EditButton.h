#pragma once

#include <QPushButton>
#include "ServerRequester.h"

class EditButton: public QPushButton {
    Q_OBJECT

    private:
        ServerRequester* server_requester;
        

    public:
        EditButton(QWidget* pwgt = nullptr);
        ~EditButton();
    
    private slots:
        void requestDialogSlot();
};