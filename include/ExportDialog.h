#pragma once

#include "BaseDialog.h"
#include <QPlainTextEdit>
#include <QLineEdit>

class ExportDialog: public BaseDialog {
    Q_OBJECT

    private:
        QPlainTextEdit* requestBody;
        QLineEdit* templateId;

    public:
        ExportDialog(QWidget* pwgt = nullptr);
        QString getName() const;
        QString getContent() const;
    
};