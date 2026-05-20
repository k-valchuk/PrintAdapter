#include "EditButton.h"
#include "EditDialog.h"

EditButton::EditButton(QWidget* pwgt): QPushButton(pwgt) {
    setText("Настройка шаблонов");
    server_requester = new ServerRequester(this, BASE_URL);
    connect(this, SIGNAL(clicked()), SLOT(requestDialogSlot()));
}

void EditButton::requestDialogSlot(){
    EditDialog* pEditDialog = new EditDialog(this, server_requester);
    pEditDialog->setAttribute(Qt::WA_DeleteOnClose); 
    pEditDialog->show(); 
}

EditButton::~EditButton(){
    delete server_requester;
}