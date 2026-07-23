#include "ShowRowDelegate.h"
#include <QPainter>
#include <QPainterPath>

void ShowRowDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QRect rect = option.rect.adjusted(0, 0, 0, -4);
    int radius = 2;
    bool isSelected = option.state & QStyle::State_Selected;
    int columnCount = index.model()->columnCount();


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
        QColor overlayColor("#456a9e"); 
        
        painter->fillPath(path, overlayColor);

        painter->setPen(QPen(overlayColor, 1));
        painter->drawPath(path);
        painter->setPen(QColor("#e0e0e0")); 
    } else {
        painter->setPen(QColor("#c2c2c2")); 
    }
    

    painter->setFont(option.font);


    QRect textRect = rect.adjusted(20, 0, -20, 0); 

    QString text = index.data(Qt::DisplayRole).toString();
    
   
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    painter->restore();
}