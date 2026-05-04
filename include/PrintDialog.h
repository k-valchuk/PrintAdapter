#pragma once
#include "PrintableReport.h"
#include <QPrintPreviewWidget>
#include <QLabel>
#include <QComboBox>
#include "BaseDialog.h"

class PrintDialog: public BaseDialog {
    Q_OBJECT

    private:
        PrintableReport* printReport;
        QPrintPreviewWidget* previewWidget;
        bool active_headers;
        QLabel* totalPagesLabel;
        QLineEdit* pageNumber;
        QComboBox* pageShow;

    public:
        PrintDialog(QWidget* pwgt, QString printContent);
    signals:
        void backButtonClicked();
};