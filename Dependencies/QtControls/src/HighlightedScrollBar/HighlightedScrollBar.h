#ifndef HIGHLIGHTEDSCROLLBAR_H
#define HIGHLIGHTEDSCROLLBAR_H

#include "QScrollBar"
#include "QAbstractItemModel"
#include "QPaintEvent"
#include "QPainter"
#include <set>

class HighlightedScrollBar : public QScrollBar
{
    QAbstractItemModel* m_model;
    std::function<bool(const QModelIndex&)> m_isHighlighted;
    std::function<bool(const QModelIndex&)> m_isExpanded;
    bool m_highlightCollapsedIfHasNeededChildren;
    std::set<int> m_selectedRows;
    bool m_highlightingEnabled = false;
    
    int m_allRowsCount = 0;
    void traverseRows(const QModelIndex& parent, int& rowCounter);
public:
    HighlightedScrollBar(QAbstractItemModel* model,
                         std::function<bool(const QModelIndex&)> isHighlighted,
                         std::function<bool(const QModelIndex&)> isExpanded,
                         bool highlightCollapsedIfHasNeededChildren = false);
    
    void setHighlightingEnabled(bool enabled);
    
    void updateHighlighting();
    
    void paintEvent(QPaintEvent* ev) override;
};

#endif // HIGHLIGHTEDSCROLLBAR_H
