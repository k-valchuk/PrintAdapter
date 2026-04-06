#pragma once

#include <QDialog>
#include <QtWidgets>

class GetTemplateDialog: public QDialog {
    Q_OBJECT

    private:
        QLineEdit* templateId;

    public:
        GetTemplateDialog(QWidget* pwgt = nullptr);
        QString getContent() const;
};