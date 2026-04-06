#pragma once

#include <QDialog>
#include <QPlainTextEdit>
#include <QLineEdit>

class ExportDialog: public QDialog {
    Q_OBJECT

    private:
        QPlainTextEdit* requestBody;
        QLineEdit* templateId;

    public:
        ExportDialog(QWidget* pwgt = nullptr);
        QString getName() const;
        QString getContent() const;
    
};