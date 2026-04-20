#pragma once
#include <QDialog>
#include <QVBoxLayout>

class BaseDialog: public QDialog {
    Q_OBJECT

    protected:
        QVBoxLayout* base_layout;
        void setBaseLayout(QLayout* layout);
        protected:
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        QPoint dragCoordinate;

    public:
        BaseDialog(QWidget* pwgt);
    

};