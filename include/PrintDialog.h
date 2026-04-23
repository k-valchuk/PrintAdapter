#pragma once
#include "PrintableReport.h"
#include "BaseDialog.h"

class PrintDialog: public BaseDialog {
    Q_OBJECT

    public:
        PrintDialog(QWidget* pwgt, PrintableReport* printReport, QPrinter* printer);
    signals:
        void paintRequested(QPrinter *);
    public slots:
        void onPaintRequested(QPrinter *printer);
};