#pragma once
#include "BaseDelegate.h"

class EditableRowDelegate : public BaseDelegate {
    Q_OBJECT
    private:
        QColor bgColor;
    public:
    EditableRowDelegate(QWidget* pwgt, QColor backgroundColor): BaseDelegate(pwgt) {
        bgColor = backgroundColor;
    };


    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(
        QEvent* event,
        QAbstractItemModel* model,
        const QStyleOptionViewItem& option,
        const QModelIndex& index
    ) override;

    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    protected:
        QRect crossRect(const QRect& rect) const;
};
