#ifndef FILTERHISTORYROW_H
#define FILTERHISTORYROW_H

#include <QFrame>
#include "DraggableFrame.h"
#include "FilterRowToolTip.h"

namespace Ui {
class FilterHistoryRow;
}


class FilterHistoryListWidget;
class FilterHistoryRow : public DraggableFrame, public FilterRowToolTip
{
    Q_OBJECT
    Q_PROPERTY(bool myHover READ myHover WRITE setMyHover NOTIFY myHoverChanged)
    Q_PROPERTY(QColor onHoverColor READ onHoverColor WRITE setOnHoverColor)
    Q_PROPERTY(QColor onHoverTextColor READ onHoverTextColor WRITE setOnHoverTextColor)
    
    FilterHistoryListWidget* m_parentList;
    bool m_myHover = false;
    bool m_isInEditMode = false;
    QColor m_onHoverColor, m_onHoverTextColor;
public:
    explicit FilterHistoryRow(FilterHistoryListWidget *parent = nullptr);
    ~FilterHistoryRow();
    
    bool myHover()
    {
        return m_myHover;
    }
    void setMyHover(bool isHover)
    {
        setMyHoverPrivate(isHover);
        emit myHoverChanged(m_myHover);
    }
    void setMyHoverSilent(bool isHover)
    {
        setMyHoverPrivate(isHover);
    }
    
    QColor onHoverColor() { return m_onHoverColor; }
    void setOnHoverColor(QColor color);
    QColor onHoverTextColor() { return m_onHoverTextColor; }
    void setOnHoverTextColor(QColor color);
private:
    Ui::FilterHistoryRow *ui;
    
    void setMyHoverPrivate(bool isHover);
public:
    bool event(QEvent* event) override;
    
public slots:
    void SetName(QString name);
signals:
    void myHoverChanged(bool hover);
    void saveClicked();
};

#endif // FILTERHISTORYROW_H
