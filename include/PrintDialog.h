#pragma once
#include "PrintableReport.h"
#include <QPrintPreviewWidget>
#include "BaseDialog.h"

class PrintDialog: public BaseDialog {
    Q_OBJECT

    private:
        PrintableReport* printReport;
        QPrintPreviewWidget* previewWidget;
        bool active_headers;

    public:
        PrintDialog(QWidget* pwgt, QString printContent);
    signals:
        void backButtonClicked();
};