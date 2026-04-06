#include "PrintButton.h"
#include "RequestDialog.h"
#include "Config.h"

PrintButton::PrintButton(QWidget* pwgt) : QPushButton(pwgt) {
    setText(PrintButtonLabel);
    server_requester = new ServerRequester(this, BASE_URL);
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