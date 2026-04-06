#pragma once

#include "BaseDialog.h"
#include <QtWidgets>

class GetElementDialog: public BaseDialog {
    Q_OBJECT

    private:
        QLineEdit* templateId;

    public:
        GetElementDialog(QWidget* pwgt = nullptr);
        QString getContent() const;
};