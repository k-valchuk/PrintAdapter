#include "PrintButton.h"
#include "RequestDialog.h"

PrintButton::PrintButton(QWidget* pwgt): QPushButton(pwgt){
    setText("Печать");
    connect(this, SIGNAL(clicked()), SLOT(requestDialogSlot()));
};

void PrintButton::requestDialogSlot() {
    RequestDialog* pRequestDialog= new RequestDialog;
    if (pRequestDialog->exec() == QDialog::Accepted){}
    delete pRequestDialog;
}