#ifndef FILTERPANEL_H
#define FILTERPANEL_H

#include <QFrame>
#include "FilterRow.h"

namespace Ui {
class FilterPanel;
}

class QuickFilterPanel;
class FilterPanel : public QFrame
{
    Q_OBJECT
    
    FilterRow* m_filterRow = nullptr;
    QSharedPointer<IExpression> m_filterRowExp = nullptr;
    QSharedPointer<IExpression> m_quickFilterExp = nullptr;
    QSharedPointer<IExpression> m_filterExp = nullptr;
    
    void updateWholeFilterExp();
public:
    explicit FilterPanel(const FilterRow::FilterRowConfig& filterRowConfig, QWidget *parent = nullptr);
    explicit FilterPanel(QWidget *parent = nullptr) : FilterPanel(FilterRow::FilterRowConfig(true, true, true, true), parent) {}
    ~FilterPanel();
    
    QSharedPointer<IExpression> CurrentFilterExpression() {return m_filterExp;}            // (облачка) И (быстрофильтр)
    QSharedPointer<IExpression> CurrentQuickFilterExpression() {return m_quickFilterExp;}  // быстрофильтр
    QSharedPointer<IExpression> CurrentFilterRowExpression() {return m_filterRowExp;}      // облачка
    
    QLayout* ContentLayout();
    QLayout* NavBarContentLayout();
    
    void SetFilterRowVisible(bool visible);
    bool IsFilterRowVisible();
    
    FilterRow* FilterRowWidget() { return m_filterRow; }
    QuickFilterPanel* QuickFilterWidget();
private:
    Ui::FilterPanel *ui;
    
signals:
    void filterChanged();
    void filterChangedData(QSharedPointer<IExpression> wholeFilter, //AND между поддеревьями quickFilter и rowFilter
                           QSharedPointer<IExpression> quickFilter,
                           QSharedPointer<IExpression> rowFilter);
    
    void filterRowVisibleChanged(bool visible);
};

#endif // FILTERPANEL_H
