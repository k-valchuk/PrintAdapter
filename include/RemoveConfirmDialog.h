#pragma once

#include "BaseDialog.h"

class RemoveConfirmDialog: public BaseDialog {
    Q_OBJECT

    public:
        RemoveConfirmDialog(QWidget* pwgt, QString templateName);
};