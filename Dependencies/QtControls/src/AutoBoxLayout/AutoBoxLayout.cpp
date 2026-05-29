#include <QtWidgets>

#include "AutoBoxLayout.h"


void AutoBoxLayout::addItem(QLayoutItem* item)
{
    m_items.append(item);
}

int AutoBoxLayout::horizontalSpacing() const
{
    if(m_hSpacing >= 0)
        return m_hSpacing;
    else
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}
int AutoBoxLayout::verticalSpacing() const
{
    if(m_vSpacing >= 0)
        return m_vSpacing;
    else
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

Qt::Orientations AutoBoxLayout::expandingDirections() const
{
    return {};
}

bool AutoBoxLayout::hasHeightForWidth() const
{
    return true;
}

int AutoBoxLayout::heightForWidth(int) const
{
    int height = doLayout(QRect(0, 0, this->geometry().width(), 0), true);
    return height;
}

int AutoBoxLayout::heightForKnownWidth(int width)
{
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

int AutoBoxLayout::count() const
{
    return m_items.size();
}

QLayoutItem* AutoBoxLayout::itemAt(int index) const
{
    return m_items.value(index);
}

QSize AutoBoxLayout::minimumSize() const
{
    int left = 0, top = 0, right = 0, bottom = 0;
    getContentsMargins(&left, &top, &right, &bottom);
    int contentsWidth = left + right, contentsHeight = top + bottom;
    int contentsMinWidth = 0, contentsMinHeight = 0;
    for(int i = 0; i < this->count(); i++)
    {
        auto item = this->itemAt(i);
        auto size = item->minimumSize();
        
        contentsWidth  += (i > 0 ? horizontalSpacing() : 0) + size.width();
        contentsHeight += (i > 0 ? verticalSpacing() : 0) + size.height();
        
        if(contentsMinWidth < size.width())
            contentsMinWidth = size.width();
        if(contentsMinHeight < size.height())
            contentsMinHeight = size.height();
    }
    
    contentsMinWidth += left + right;
    contentsMinHeight += top + bottom;
    
    auto rect = this->parentWidget()->contentsRect();
    if(m_horizontalPriority){
        if(rect.height() >= contentsHeight)
            return QSize(contentsMinWidth, contentsMinHeight);
        else
            return QSize(contentsWidth, contentsMinHeight);
    }
    else{
        if(rect.width() >= contentsWidth)
            return QSize(contentsMinWidth, contentsMinHeight);
        else
            return QSize(contentsMinWidth, contentsHeight);
    }
}

void AutoBoxLayout::setGeometry(const QRect& rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize AutoBoxLayout::sizeHint() const
{
    return minimumSize();
}

QLayoutItem* AutoBoxLayout::takeAt(int index)
{
    if (index >= 0 && index < m_items.size())
        return m_items.takeAt(index);
    return nullptr;
}

void AutoBoxLayout::setAlignmentWhenHorizontal(Alignment main, Alignment baseline, SideAlignment side)
{
    m_alignmentWhenH = AlignmentState{main, baseline, side};
    invalidate();
}

void AutoBoxLayout::setAlignmentWhenVertical(Alignment main, Alignment baseline, SideAlignment side)
{
    m_alignmentWhenV = AlignmentState{main, baseline, side};
    invalidate();
}

void AutoBoxLayout::setSpacing(int hSpacing, int vSpacing)
{
    m_hSpacing = hSpacing;
    m_vSpacing = vSpacing;
    invalidate();
}

void AutoBoxLayout::setPriorityOrientation(bool horizontal)
{
    m_horizontalPriority = horizontal;
    invalidate();
}

bool AutoBoxLayout::shouldBeHorizontal(QRect rect) const
{
    int left = 0, top = 0, right = 0, bottom = 0;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    
    int contentsWidth = 0, contentsHeight = 0;
    for(int i = 0; i < this->count(); i++)
    {
        auto item = this->itemAt(i);
        auto size = item->minimumSize();
        
        contentsWidth  += (i > 0 ? this->horizontalSpacing() : 0) + size.width();
        contentsHeight += (i > 0 ? this->verticalSpacing() : 0) + size.height();
    }
    
    if(m_horizontalPriority){
        if(effectiveRect.width() <= contentsWidth && effectiveRect.height() >= contentsHeight)
            return false;
        else
            return true;
    }
    else {
        if(effectiveRect.height() <= contentsHeight && effectiveRect.width() >= contentsWidth)
            return true;
        else
            return false;
    }
}

int AutoBoxLayout::doLayout(QRect rect, bool testOnly) const
{
    auto horizontal = shouldBeHorizontal(rect);
    
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    
    int contentsWidth = 0, contentsHeight = 0;
    int contentsMinWidth = 0, contentsMinHeight = 0;
    for(int i = 0; i < this->count(); i++)
    {
        auto item = this->itemAt(i);
        auto size = item->minimumSize();
        
        contentsWidth  += (i > 0 ? this->horizontalSpacing() : 0) + size.width();
        contentsHeight += (i > 0 ? this->verticalSpacing() : 0) + size.height();
        
        if(contentsMinWidth < size.width())
            contentsMinWidth = size.width();
        if(contentsMinHeight < size.height())
            contentsMinHeight = size.height();
    }
    
    int resultHeight = 0;
    QList<QPair<QLayoutItem*, QRect>> itemToGeometry;
    if(horizontal) {
        auto totalStretch = 0;
        for(int i = 0; i < this->count(); i++)
        {
            auto item = this->itemAt(i);
            auto wgt = item->widget();
            if(wgt) {
                auto hstretch = wgt->sizePolicy().horizontalStretch();
                totalStretch += hstretch;
            }
        }
        
        int nonExpansivesWidth = 0;
        int expansivesCount = 0;
        int emptySpace = effectiveRect.width() - horizontalSpacing()*(count()-1);
        int maxItemHeight = 0;
        for(int i = 0; i < this->count(); i++)
        {
            auto item = this->itemAt(i);
            auto wgt = item->widget();
            if(wgt) {
                auto hsizepolicy = wgt->sizePolicy().horizontalPolicy();
                auto hstretch = wgt->sizePolicy().horizontalStretch();
                if((hsizepolicy & QSizePolicy::GrowFlag) && (hstretch > 0 || totalStretch == 0))
                {
                    expansivesCount++;
                    auto maxWidth = item->maximumSize().width();
                    emptySpace -= qMin(maxWidth, emptySpace);
                }
                else
                {
                    auto width = item->sizeHint().width();
                    nonExpansivesWidth += width;
                    emptySpace -= width;
                }
                
                auto vsizepolicy = wgt->sizePolicy().verticalPolicy();
                int itemHeight = 0;
                if(vsizepolicy & QSizePolicy::GrowFlag)
                    itemHeight = qMin(item->maximumSize().height(), effectiveRect.height());
                else
                    itemHeight = item->sizeHint().height();
                if(itemHeight > maxItemHeight)
                    maxItemHeight = itemHeight;
            }
        }
        if(emptySpace < 0) emptySpace = 0;
        
        int innerSpacing = horizontalSpacing();
        int leftSpacing = 0, rightSpacing = 0;
        if(m_alignmentWhenH.side == SideAlignment::Center)
        {
            leftSpacing = rightSpacing = qRound(emptySpace/2.0);
        }
        else if(m_alignmentWhenH.side == SideAlignment::Width)
        {
            leftSpacing = 0; rightSpacing = 0;
            innerSpacing = horizontalSpacing() + qRound((double)emptySpace/(count() - 1));
        }
        else if(m_alignmentWhenH.side == SideAlignment::WidthAround)
        {
            auto space = qRound((double)emptySpace/(count() + 1));
            leftSpacing = rightSpacing = space;
            innerSpacing = horizontalSpacing() + space;
        }
        else if(m_alignmentWhenH.side == SideAlignment::Start)
        {
            leftSpacing = 0;
            rightSpacing = emptySpace;
        }
        else if(m_alignmentWhenH.side == SideAlignment::End)
        {
            leftSpacing = emptySpace;
            rightSpacing = 0;
        }
        
        nonExpansivesWidth += innerSpacing*(count()-1) + leftSpacing + rightSpacing;
        
        auto baseline = effectiveRect.center().y();
        if(m_alignmentWhenH.main == Alignment::Start)
            baseline = effectiveRect.top() + maxItemHeight/2;
        else if(m_alignmentWhenH.main == Alignment::End)
            baseline = effectiveRect.bottom() - maxItemHeight/2;
        
        int x = effectiveRect.x() + leftSpacing;
        for(int i = 0; i < this->count(); i++)
        {
            auto item = this->itemAt(i);
            auto maxSize = item->maximumSize();
            auto size = item->sizeHint();
            auto wgt = item->widget();
            if(wgt) {
                auto hsizepolicy = wgt->sizePolicy().horizontalPolicy();
                auto vsizepolicy = wgt->sizePolicy().verticalPolicy();
                auto hstretch    = wgt->sizePolicy().horizontalStretch();
                
                if((hsizepolicy & QSizePolicy::GrowFlag) && (hstretch > 0 || totalStretch == 0))
                {
                    int s1 = 0, s2 = maxSize.width();
                    if(totalStretch == 0)
                        s1 = qRound((effectiveRect.width() - nonExpansivesWidth)/(double)expansivesCount);
                    else
                        s1 = qRound(hstretch*(effectiveRect.width() - nonExpansivesWidth)/(double)totalStretch);
                    
                    size.setWidth(qMin(s1, s2));
                    nonExpansivesWidth += size.width();
                    expansivesCount--;
                    totalStretch -= hstretch;
                }
                
                if(vsizepolicy & QSizePolicy::GrowFlag)
                    size.setHeight(qMin(effectiveRect.height(), maxSize.height()));
            }
            
            int y = baseline - size.height()/2;
            if(m_alignmentWhenH.baseline == Alignment::Start)
                y = baseline - maxItemHeight/2;
            else if(m_alignmentWhenH.baseline == Alignment::End)
                y = baseline + maxItemHeight/2 - size.height();
            itemToGeometry.append(QPair<QLayoutItem*, QRect>{item, QRect(x, y, size.width(), size.height())});
            
            if(y < effectiveRect.y())
            {
                size.setHeight(size.height() - (effectiveRect.y() - y));
                y = effectiveRect.y();
            }
            if(y > effectiveRect.bottom())
            {
                size.setHeight(size.height() - (y - effectiveRect.bottom()));
                y = effectiveRect.bottom();
            }
            
            x += size.width() + innerSpacing;
        }
        
        resultHeight = effectiveRect.top() + maxItemHeight;
    }
    else {
        auto totalStretch = 0;
        for(int i = 0; i < this->count(); i++)
        {
            auto item = this->itemAt(i);
            auto wgt = item->widget();
            if(wgt) {
                auto vstretch = wgt->sizePolicy().verticalStretch();
                totalStretch += vstretch;
            }
        }
        
        int nonExpansivesHeight = 0;
        int expansivesCount = 0;
        int emptySpace = effectiveRect.height() - verticalSpacing()*(count()-1);
        int maxItemWidth = 0;
        
        for(int i = 0; i < this->count(); i++)
        {
            auto item = this->itemAt(i);
            auto wgt = item->widget();
            if(wgt) {
                auto vsizepolicy = wgt->sizePolicy().verticalPolicy();
                auto vstretch = wgt->sizePolicy().verticalStretch();
                if((vsizepolicy & QSizePolicy::GrowFlag) && (vstretch > 0 || totalStretch == 0))
                {
                    expansivesCount++;
                    auto maxHeight = item->maximumSize().height();
                    emptySpace -= qMin(maxHeight, emptySpace);
                }
                else
                {
                    auto height = item->sizeHint().height();
                    nonExpansivesHeight += height;
                    emptySpace -= height;
                }
                
                auto hsizepolicy = wgt->sizePolicy().horizontalPolicy();
                int itemWidth = 0;
                if(hsizepolicy & QSizePolicy::GrowFlag)
                    itemWidth = qMin(item->maximumSize().width(), effectiveRect.width());
                else
                    itemWidth = item->sizeHint().width();
                if(hsizepolicy > maxItemWidth)
                    maxItemWidth = itemWidth;
            }
        }
        if(emptySpace < 0) emptySpace = 0;
        
        int innerSpacing = verticalSpacing();
        int topSpacing = 0, bottomSpacing = 0;
        if(m_alignmentWhenV.side == SideAlignment::Center)
        {
            topSpacing = bottomSpacing = emptySpace/2;
        }
        else if(m_alignmentWhenV.side == SideAlignment::Width)
        {
            topSpacing = 0; bottomSpacing = 0;
            innerSpacing += emptySpace/(count() - 1);
        }
        else if(m_alignmentWhenV.side == SideAlignment::WidthAround)
        {
            innerSpacing = topSpacing = bottomSpacing = verticalSpacing() + emptySpace/(count() + 1);
        }
        else if(m_alignmentWhenV.side == SideAlignment::Start)
        {
            topSpacing = 0;
            bottomSpacing = emptySpace;
        }
        else if(m_alignmentWhenV.side == SideAlignment::End)
        {
            topSpacing = emptySpace;
            bottomSpacing = 0;
        }
        
        nonExpansivesHeight += innerSpacing*(count()-1) + topSpacing + bottomSpacing;
        
        auto baseline = effectiveRect.center().x();
        if(m_alignmentWhenV.main == Alignment::Start)
            baseline = effectiveRect.top() + maxItemWidth/2;
        else if(m_alignmentWhenV.main == Alignment::End)
            baseline = effectiveRect.bottom() - maxItemWidth/2;
        
        int y = effectiveRect.y() + topSpacing;
        for(int i = 0; i < this->count(); i++)
        {
            auto item = this->itemAt(i);
            auto maxSize = item->maximumSize();
            auto size = item->sizeHint();
            auto wgt = item->widget();
            if(wgt) {
                auto vsizepolicy = wgt->sizePolicy().verticalPolicy();
                auto hsizepolicy = wgt->sizePolicy().horizontalPolicy();
                auto vstretch    = wgt->sizePolicy().verticalStretch();
                
                if((vsizepolicy & QSizePolicy::GrowFlag) && (vstretch > 0 || totalStretch == 0))
                {
                    int s1 = 0, s2 = maxSize.height();
                    if(totalStretch == 0)
                        s1 = qRound((effectiveRect.height() - nonExpansivesHeight)/(double)expansivesCount);
                    else
                        s1 = qRound(vstretch*(effectiveRect.height() - nonExpansivesHeight)/(double)totalStretch);
                    
                    size.setHeight(qMin(s1, s2));
                    nonExpansivesHeight += size.height();
                    expansivesCount--;
                    totalStretch -= vstretch;
                }
                
                if(hsizepolicy & QSizePolicy::GrowFlag)
                    size.setWidth(qMin(effectiveRect.width(), maxSize.width()));
            }
            
            int x = baseline - size.width()/2;
            if(m_alignmentWhenV.baseline == Alignment::Start)
                x = baseline - maxItemWidth/2;
            else if(m_alignmentWhenV.baseline == Alignment::End)
                x = baseline + maxItemWidth/2 - size.width();
            
            if(x < effectiveRect.x())
            {
                size.setWidth(size.width() - (effectiveRect.x() - x));
                x = effectiveRect.x();
            }
            if(x > effectiveRect.right())
            {
                size.setWidth(size.width() - (x - effectiveRect.right()));
                x = effectiveRect.right();
            }
            
            itemToGeometry.append(QPair<QLayoutItem*, QRect>{item, QRect(x, y, size.width(), size.height())});
            
            y += size.height() + innerSpacing;
        }
        
        resultHeight = y - innerSpacing;
    }
    
    if(!testOnly)
    {
        for(auto& item : itemToGeometry)
            item.first->setGeometry(item.second);
    }
    
    return effectiveRect.height();//resultHeight;
}

int AutoBoxLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}
