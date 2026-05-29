#include "HighlightedScrollBar.h"

#include "qmath.h"

void HighlightedScrollBar::traverseRows(const QModelIndex& parent, int& rowCounter)
{   
    if(parent.isValid() && m_isHighlighted && m_isHighlighted(parent))
        m_selectedRows.insert(rowCounter);
    
    rowCounter += (parent.isValid() ? 1 : 0);
    
    bool collapsed = parent.isValid() && m_isExpanded && m_isExpanded(parent) == false;
    if(collapsed && !m_highlightCollapsedIfHasNeededChildren)
        return;
    else if(collapsed && m_highlightCollapsedIfHasNeededChildren)
    {
        bool foundHighlighted = false;
        std::function<bool(const QModelIndex&)> func;
        func = [&](const QModelIndex& idx)->bool{
            auto cnt = m_model->rowCount(idx);
            for(int i = 0; i < cnt; i++)
            {
                auto index = m_model->index(i, 0, idx);
                if(m_isHighlighted && m_isHighlighted(index))
                {
                    foundHighlighted = true;
                    return false;
                }
                if(func(index) == false)
                    return false;
            }
            return true;
        };
        
        func(parent);
        if(foundHighlighted)
            m_selectedRows.insert(rowCounter - 1);
        
        return;
    }
    
    auto cnt = m_model->rowCount(parent);
    for(int i = 0; i < cnt; i++)
    {
        auto index = m_model->index(i, 0, parent);
        
        if(m_isHighlighted && m_isHighlighted(index))
            m_selectedRows.insert(rowCounter);
        
        traverseRows(index, rowCounter);
    }
}

HighlightedScrollBar::HighlightedScrollBar(QAbstractItemModel* model, std::function<bool (const QModelIndex&)> isHighlighted, std::function<bool (const QModelIndex&)> isExpanded, bool highlightCollapsedIfHasNeededChildren)
{
    m_model = model;
    m_isHighlighted = isHighlighted;
    m_isExpanded = isExpanded;
    m_highlightCollapsedIfHasNeededChildren = highlightCollapsedIfHasNeededChildren;
}

void HighlightedScrollBar::setHighlightingEnabled(bool enabled)
{
    m_highlightingEnabled = enabled;
    update();
}

void HighlightedScrollBar::updateHighlighting()
{
    m_selectedRows.clear();
    m_allRowsCount = 0;
    traverseRows(QModelIndex(), m_allRowsCount);
    update();
}

void HighlightedScrollBar::paintEvent(QPaintEvent* ev)
{
    QScrollBar::paintEvent(ev);
    
    if(m_allRowsCount < 1)
        return;
    if(m_selectedRows.size() < 1)
        return;
    
    if(!m_highlightingEnabled)
        return;
    
    QPainter p(this);
    
    auto rect = this->rect();
    auto height = rect.height();
    
    int rcHeight = qCeil(height / (double)m_allRowsCount);
    
    int maxY = 0;
    for(auto row : m_selectedRows)
    {
        auto rowRect = ev->rect();
        auto pos = qRound(row * height/(double)m_allRowsCount) + rect.top();
        rowRect.setTop(qMax(maxY+1, pos));
        rowRect.setBottom(pos + rcHeight);
        maxY = rowRect.bottom();
        
        p.fillRect(rowRect, QColor::fromRgb(0, 150, 0, 70));
    }
}
