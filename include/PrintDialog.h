#pragma once
#include "PrintableReport.h"
#include <QPrintPreviewWidget>
#include <QLabel>
#include <QComboBox>
#include "BaseDialog.h"
#include "PageSetupDialog.h"

class PrintDialog: public BaseDialog {
    Q_OBJECT

    private:
        PrintableReport* printReport;
        QPrintPreviewWidget* previewWidget;
        bool active_headers;
        QLabel* totalPagesLabel;
        QLineEdit* pageNumber;
        QComboBox* pageShow;
        QString headerTitle;
        QList<QList<Headers>> headersValues;

    public:
        PrintDialog(QWidget* pwgt, QString printContent, QString rundownTitle);
    
    public slots:
        void updateHeaders(QStringList headers, QStringList footers, QList<QList<Headers>> newHeadersValues);
};