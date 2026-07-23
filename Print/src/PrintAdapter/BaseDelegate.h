#pragma once
#include <QStyledItemDelegate>

class BaseDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
        BaseDelegate(QWidget* pwgt): QStyledItemDelegate(pwgt){}

    signals:
        void removeRequested(int row);

};