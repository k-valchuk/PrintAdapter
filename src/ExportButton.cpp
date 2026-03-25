#include "ExportButton.h"
#include "ExportDialog.h"

ExportButton::ExportButton(QWidget* pwgt): QPushButton(pwgt) {
    connect(this, SIGNAL(clicked()), SLOT(exportDialogSlot()));
}

void ExportButton::exportDialogSlot() {
    ExportDialog* pExportDialog = new ExportDialog;
    if (pExportDialog->exec() == QDialog::Accepted){
        emit done(pExportDialog->getContent(), ContentType::HTML);
    }
    delete pExportDialog;
}