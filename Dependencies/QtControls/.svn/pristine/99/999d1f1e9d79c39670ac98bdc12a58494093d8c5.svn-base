#include "dropIndicatorProxyStyle.h"
#include "QStyleOption"
#include "QPainter"
#include "QTreeView"

DropIndicatorProxyStyle::DropIndicatorProxyStyle(const QColor &diColor, QStyle* style) : QProxyStyle(style), dropIndicatorColor(diColor)
{

}

void DropIndicatorProxyStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::PE_IndicatorItemViewItemDrop && !option->rect.isNull() && widget)
    {//отрисовка индикатора
        const int rad = 3;
        QStyleOption opt(*option);

        const QTreeView *trView = qobject_cast<const QTreeView*>(widget);
        int leftPadding = 0;
        if (trView)
        {// Рассчет отступа для дочерних элементов
            int countParents = 0;
            QModelIndex parInd = trView->indexAt(option->rect.topLeft()).parent();
            while (parInd.isValid())
            {
                ++countParents;
                parInd = parInd.parent();
            }
            leftPadding = trView->indentation() * countParents;
        }

        opt.rect.setLeft(rad + leftPadding);
        opt.rect.setRight(widget->width() - 2 * rad);

        painter->setRenderHint(QPainter::Antialiasing, true);

        QColor color(dropIndicatorColor);
        QPen pen(color);
        pen.setWidth(2);
        color.setAlpha(50);
        QBrush brush(color);
        painter->setPen(pen);
        painter->setBrush(brush);
        if (opt.rect.height() == 0)//линия
        {
            painter->drawEllipse(opt.rect.topLeft(), rad, rad);
            painter->drawLine(opt.rect.topLeft().x() + rad, opt.rect.topLeft().y(), opt.rect.topRight().x() - rad, opt.rect.topRight().y());
            painter->drawEllipse(opt.rect.topRight(), rad, rad);
        }
        else //прямоугольник
        {
            painter->drawRoundedRect(opt.rect, rad, rad);
        }

        return;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
}
