#pragma once

#include <QDialog>
#include <QPlainTextEdit>
#include <QLineEdit>

class ExportDialog: public QDialog {
    Q_OBJECT

    private:
        QPlainTextEdit* requestBody;
        QLineEdit* templateName;

    public:
        ExportDialog(QWidget* pwgt = nullptr);
        QString getContent() const;
    
};