#pragma once

#include <QPushButton>
#include "RequestDialog.h"

class ExportButton: public QPushButton {
    Q_OBJECT

    public:
        ExportButton(QWidget* pwgt = nullptr);
    
    private slots:
        void exportDialogSlot();
    
    signals:
        void done(QString content, ContentType content_type);
};