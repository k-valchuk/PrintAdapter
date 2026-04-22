#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QFrame>
#include <QString>

class BaseDialog: public QDialog {
    Q_OBJECT

    protected:
        bool m_dragged;
        QFrame* titleBar;
        QVBoxLayout* base_layout;
        void setBaseLayout(QLayout* layout);
        protected:
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        QPoint dragCoordinate;

    public:
        BaseDialog(QWidget* pwgt, QString title);

};