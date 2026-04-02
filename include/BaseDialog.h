#pragma once
#include <QDialog>
#include <QVBoxLayout>

class BaseDialog: public QDialog {
    Q_OBJECT

    protected:
        QVBoxLayout* base_layout;
        void setBaseLayout(QLayout* layout);

    public:
        BaseDialog(QWidget* pwgt);
};