#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QFrame>
#include <QString>

class BaseDialog: public QDialog {
    Q_OBJECT

    protected:
        QVBoxLayout* base_layout;
        void setBaseLayout(QLayout* layout);

    public:
        BaseDialog(QWidget* pwgt, QString title);

};