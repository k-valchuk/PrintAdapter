#ifndef FILTERELLIPSE_H
#define FILTERELLIPSE_H

#include <QFrame>
#include <QLabel>
#include "IFilterElement.h"
#include "FiltersList.h"
#include "qcoreevent.h"
#include <QMouseEvent>
#include "OperatorList.h"
#include "qdebug.h"

namespace Ui {
class FilterEllipse;
}

class FilterRow;

class ElidedLabel;
class FilterEllipse : public QFrame
{
    Q_OBJECT
    
    FilterRow* m_parentFilter;
    
    QSharedPointer<IFilterElement> m_selectedElement;
    QVariant m_selectedOperator = QVariant();
    QWidget* m_editorWgt = nullptr;
    ElidedLabel* m_valueLabel;
    QVariant m_value = QVariant();
    FiltersList* m_filtersList = nullptr;
    OperatorList* m_opsList = nullptr;
    bool m_blockOnFocusChange = false;
    
    void updateFilter(QSharedPointer<IFilterElement> oldFilter, QVariant oldOperator);
    
    bool m_isExpanded = false;
    void shrink();
    void expand();
    
    void prepareEditorWgt();
    
    bool isChildOf(QWidget* child, QWidget* parent)
    {
        while(parent)
        {
            if(parent == child)
                return true;
            parent = parent->parentWidget();
        }
        return false;
    }
    
    QString getTextForValueLabel();
public:
    FilterEllipse(FilterRow* m_parentFilter);
    ~FilterEllipse();
    
    void SetColor(QColor color);
    
    bool IsAllFieldsFilter();
    QVariant GetValue();
    QString GetDisplayValue();
    
    void SetValue(QVariant value);
    void selectFilter(QSharedPointer<IFilterElement> element);
    void selectFilter(QVariant filterKey);
    void selectOperator(QVariant opKey);
    
    QSharedPointer<IFilterElement> selectedFilter()
    {
        return m_selectedElement;
    }
    QVariant selectedOperator()
    {
        return m_selectedOperator;
    }
    
    QVariant GetFilterExpression();
    
    void Delete()
    {
        emit elementDeleted(this);
    }
    
    bool IsExpanded();
private:
    Ui::FilterEllipse *ui;
    
protected:
    void moveEvent(QMoveEvent* event) override;
    
private slots:
    void filtersListClicked();
    void opsListClicked();
    void deleteElementClicked();
    void focusChanged(QWidget* old, QWidget* now);
signals:
    void elementDeleted(FilterEllipse* element);
    void selectedFilterChanged(FilterEllipse* element, QSharedPointer<IFilterElement> selected, QVariant selectedOp);
    void valueChanged(FilterEllipse* element, QSharedPointer<IFilterElement> selected, QVariant selectedOp,
                     QVariant old, QVariant value); 
};


#endif // FILTERELLIPSE_H
