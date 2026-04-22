#pragma once
#include <QPrintPreviewDialog>
#include "BaseDialog.h"

class PrintDialog: public BaseDialog {
    Q_OBJECT

    public:
        PrintDialog(QWidget* pwgt, QPrinter* printer);
    signals:
        void paintRequested(QPrinter *);
    public slots:
        void onPaintRequested(QPrinter *printer);
};