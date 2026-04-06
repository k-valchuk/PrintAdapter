#include "PrintButton.h"
#include "RequestDialog.h"

PrintButton::PrintButton(QWidget* pwgt) : QPushButton(pwgt) {
    setText("Печать!!");
    server_requester = new ServerRequester(this, QString("http://localhost:8000"));
    connect(this, SIGNAL(clicked()), SLOT(requestDialogSlot()));
};

void PrintButton::requestDialogSlot() {
    RequestDialog* pRequestDialog= new RequestDialog(this, server_requester);
    if (pRequestDialog->exec() == QDialog::Accepted){}
    delete pRequestDialog;
}
PrintButton::~PrintButton() {
    delete server_requester;
}