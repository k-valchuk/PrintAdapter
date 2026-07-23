#pragma once
#include "BaseDelegate.h"


class ShowRowDelegate : public BaseDelegate {
    Q_OBJECT
    private:
        QColor bgColor;
    public:
    ShowRowDelegate(QWidget* pwgt, QColor backgroundColor): BaseDelegate(pwgt) {
        bgColor = backgroundColor;
    };

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        return nullptr; 
    }


    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};