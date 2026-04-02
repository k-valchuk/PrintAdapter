#pragma once

#include "BaseDialog.h"
#include <QPlainTextEdit>
#include <QLineEdit>

class TagDialog: public BaseDialog {
    Q_OBJECT

    private:
        QLineEdit* tagName;
        QPlainTextEdit* tagDescription;
        QLineEdit* tagSubsystem;
        QLineEdit* tagAlias;

    public:
        TagDialog(QWidget* pwgt = nullptr);
        QString getName() const;
        QString getDescription() const;
        QString getSubsystem() const;
        QString getAlias() const;

};