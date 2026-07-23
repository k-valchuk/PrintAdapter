#include "RowDelegate.h"
#include <QPainter>
#include <QPainterPath>
#include <QEvent>
#include <QMouseEvent>
#include <QToolTip>

void EditableRowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QRect rect = option.rect.adjusted(0, 0, 0, -4);
    int radius = 2;
    bool isSelected = option.state & QStyle::State_Selected;
    int columnCount = index.model()->columnCount();


    QPainterPath path;
    painter->fillPath(path, bgColor);
    painter->fillRect(rect, bgColor);

    if (isSelected) {
        QColor overlayColor("#456A9E"); 
        
        painter->fillPath(path, overlayColor);
        painter->fillRect(rect.adjusted(0, 0, 0, 0), overlayColor);

        painter->setPen(QPen(overlayColor));
        painter->drawPath(path);
    }


    painter->setPen(QColor("#C2C2C2")); 
    if (isSelected) {
        painter->setPen(QColor("#E0E0E0")); 
    }
    

    painter->setFont(option.font);


    QRect textRect = rect.adjusted(20, 0, -20, 0); 

    QString text = index.data(Qt::DisplayRole).toString();
    
   
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    if ((option.state & QStyle::State_MouseOver) && index.column() == index.model()->columnCount()- 1){
        QRect icon =
            crossRect(rect);
        
        QPoint mousePos = QCursor::pos();
        if (option.widget) {
            mousePos = option.widget->mapFromGlobal(QCursor::pos());
        }


        if (icon.contains(mousePos)) {
            painter->setPen(
                QColor("#ff7272")
            );
        } else {
            painter->setPen(
                QColor("#cc5b5b")
            );
        }
        

        painter->drawText(
            icon,
            Qt::AlignCenter,
            "✕"
        );
    }

    painter->restore();
}

void EditableRowDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QRect cellRect = option.rect.adjusted(0, 0, 0, -4);


    int desiredWidth = cellRect.width() - 4; 
    int desiredHeight = 28; 

    QRect editorRect = QStyle::alignedRect(
        Qt::LeftToRight,           
        Qt::AlignCenter,           
        QSize(desiredWidth, desiredHeight), 
        cellRect                   
    );

    editor->setGeometry(editorRect);
}

bool EditableRowDelegate::editorEvent(
        QEvent* event,
        QAbstractItemModel* model,
        const QStyleOptionViewItem& option,
        const QModelIndex& index
){
    if (index.column() != model->columnCount() - 1){
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    if (event->type() == QEvent::MouseMove) {
        auto* mouse = static_cast<QMouseEvent*>(event);
        QRect iconRect = crossRect(option.rect);

        if (iconRect.contains(mouse->pos())) {
            QToolTip::showText(QCursor::pos(), QString::fromUtf8("Удалить шаблон"), const_cast<QWidget*>(option.widget));
        } else {
            QToolTip::hideText();
        }


        if (option.widget) {
            const_cast<QWidget*>(option.widget)->update(option.rect);
        }
        return true; 
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        auto* mouse = static_cast<QMouseEvent*>(event);
        if (mouse && crossRect(option.rect).contains(mouse->pos())) {
            QToolTip::hideText(); 
            emit removeRequested(index.row());
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool EditableRowDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, 
               const QStyleOptionViewItem &option, const QModelIndex &index) 
{
    if (event->type() == QEvent::ToolTip && index.column() == index.model()->columnCount() - 1) {
        QRect iconRect = crossRect(option.rect);
        
        if (iconRect.contains(event->pos())) {
            QWidget* viewWidget = (QWidget*)view;
            QToolTip::showText(event->globalPos(), "Удалить шаблон", viewWidget);
            return true;
        }
    }
    return QStyledItemDelegate::helpEvent(event, view, option, index);
}

QRect EditableRowDelegate::crossRect(const QRect& rect) const {
    return QRect(
        rect.right() - 24,
        rect.center().y() - 7,
        16,
        16
    );
}