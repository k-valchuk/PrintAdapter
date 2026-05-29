#include "AspectRatioSingleItemLayout.h"
#include "QWidget"

AspectRatioSingleItemLayout::AspectRatioSingleItemLayout(QWidget *parent, double aspR)
    : QLayout(parent), layItem(nullptr), aspectRatio(aspR)
{
}

AspectRatioSingleItemLayout::~AspectRatioSingleItemLayout()
{
    delete layItem;
}

void AspectRatioSingleItemLayout::setAspectRatio(double aspR)
{
    aspectRatio = aspR;
}

int AspectRatioSingleItemLayout::count() const
{
    return layItem != nullptr ? 1 : 0;
}

QLayoutItem *AspectRatioSingleItemLayout::itemAt(int i) const
{
    return i == 0 ? layItem : nullptr;
}

QLayoutItem *AspectRatioSingleItemLayout::takeAt(int)
{
    QLayoutItem *retval = layItem;
    layItem = nullptr;
    return retval;
}

Qt::Orientations AspectRatioSingleItemLayout::expandingDirections() const
{
    return  Qt::Horizontal | Qt::Vertical;
}

bool AspectRatioSingleItemLayout::hasHeightForWidth() const
{
    return false;
}

int AspectRatioSingleItemLayout::heightForWidth(int width) const
{
    int height = (width - 2 * margin()) / aspectRatio + 2 * margin();
    return height;
}

void AspectRatioSingleItemLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    if (layItem)
    {
        QWidget *wdg = layItem->widget();
        int availW = rect.width() - 2 * margin();
        int availH = rect.height() - 2 * margin();
        int w, h;
        h = availH;
        w = h * aspectRatio;
        if (w > availW)
        {
            // fill width
            w = availW;
            h = w / aspectRatio;
            int y;
            if (layItem->alignment() & Qt::AlignTop)
                y = margin();
            else if (layItem->alignment() & Qt::AlignBottom)
                y = rect.height() - margin() - h;
            else
                y = margin() + (availH - h) / 2;
            wdg->setGeometry(rect.x() + margin(), rect.y() + y, w, h);
        }
        else
        {
            int x;
            if (layItem->alignment() & Qt::AlignLeft)
                x = margin();
            else if (layItem->alignment() & Qt::AlignRight)
                x = rect.width() - margin() - w;
            else
                x = margin() + (availW - w) / 2;
            wdg->setGeometry(rect.x() + x, rect.y() + margin(), w, h);
        }
    }
}

QSize AspectRatioSingleItemLayout:: AspectRatioSingleItemLayout::sizeHint() const
{
    const int margins = 2 * margin();
    return layItem ? layItem->sizeHint() + QSize(margins, margins) : QSize(margins, margins);
}

QSize AspectRatioSingleItemLayout:: AspectRatioSingleItemLayout::minimumSize() const
{
    const int margins = 2 * margin();
    return layItem ? layItem->minimumSize() + QSize(margins, margins) : QSize(margins,margins);
}

void AspectRatioSingleItemLayout::addItem(QLayoutItem *item)
{
    delete layItem;
    layItem = item;
    item->setAlignment(0);
}
