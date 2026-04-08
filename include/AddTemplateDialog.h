#pragma once

#include "BaseDialog.h"
#include <QPlainTextEdit>
#include <QLineEdit>

class AddTemplateDialog: public BaseDialog {
    Q_OBJECT

    private:
        QPlainTextEdit* requestBody;
        QLineEdit* templateName;

    public:
        AddTemplateDialog(QWidget* pwgt = nullptr);
        QString getName() const;
        QString getContent() const;
    
};