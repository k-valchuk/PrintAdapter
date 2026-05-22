#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QEvent>
#include <QMouseEvent>

class RowDelegate : public QStyledItemDelegate {
    Q_OBJECT
    private:
        QColor bgColor;
        bool editable;
    public:
    RowDelegate(QWidget* pwgt, QColor backgroundColor, bool editable_): QStyledItemDelegate(pwgt) {
        bgColor = backgroundColor;
        editable = editable_;
    };

    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QRect rect = option.rect.adjusted(0, 0, 0, -4);
    int radius = 2;
    bool isSelected = option.state & QStyle::State_Selected;


    QPainterPath path;
    if (index.column() == 0) {
        path.addRoundedRect(rect, radius, radius);
        painter->fillPath(path, bgColor);
        painter->fillRect(rect.adjusted(radius, 0, 0, 0), bgColor);
    } else if (index.column() == index.model()->columnCount() - 1) {
        path.addRoundedRect(rect, radius, radius);
        painter->fillPath(path, bgColor);
        painter->fillRect(rect.adjusted(0, 0, -radius, 0), bgColor);
    } else {
        painter->fillRect(rect, bgColor);
    }

    if (isSelected) {
        QColor overlayColor(111, 140, 183, 24); 
        
        painter->fillPath(path, overlayColor);

        painter->setPen(QPen(QColor(111, 140, 183), 1));
        painter->drawPath(path);
    }


    painter->setPen(QColor("#BABABA")); 
    

    painter->setFont(option.font);


    QRect textRect = rect.adjusted(20, 0, -20, 0); 

    QString text = index.data(Qt::DisplayRole).toString();
    
   
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    if (editable && (option.state & QStyle::State_MouseOver) && index.column() == index.model()->columnCount()- 1){
        QRect icon =
            crossRect(rect);

        painter->setPen(
            QColor("#FF7272")
        );

        painter->drawText(
            icon,
            Qt::AlignCenter,
            "✕"
        );
    }

    painter->restore();
    }

    bool editorEvent(
        QEvent* event,
        QAbstractItemModel* model,
        const QStyleOptionViewItem& option,
        const QModelIndex& index
    ) override {
        if (index.column() != index.model()->columnCount()- 1){
            return false;
        }

        if (event->type() != QEvent::MouseButtonRelease){
            return false;
        }

        auto* mouse = static_cast<QMouseEvent*>(event);
        if (!mouse) {
            return false;
        }

        if (crossRect(option.rect).contains(mouse->pos())){
            emit removeRequested(
                index.row()
            );

            return true;
        }

        return false;
    }
signals:
    void removeRequested(int row);
protected:
    QRect crossRect(
        const QRect& rect
    ) const
    {
        return QRect(
            rect.right() - 24,
            rect.center().y() - 8,
            16,
            16
        );
    }
};
