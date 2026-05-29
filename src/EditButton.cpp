#include "EditButton.h"
#include "EditDialog.h"

EditButton::EditButton(QWidget* pwgt): QPushButton(pwgt) {
    setText("Настройка шаблонов");
    connect(this, SIGNAL(clicked()), SLOT(requestDialogSlot()));
}

void EditButton::requestDialogSlot(){
    EditDialog* pEditDialog = new EditDialog(this);
    pEditDialog->setAttribute(Qt::WA_DeleteOnClose); 
    pEditDialog->show(); 
}

EditButton::~EditButton(){
    delete server_requester;
}