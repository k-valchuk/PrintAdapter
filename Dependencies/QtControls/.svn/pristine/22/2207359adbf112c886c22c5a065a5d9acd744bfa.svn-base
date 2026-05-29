
#ifndef FILTERROWNODE_H
#define FILTERROWNODE_H

#include <QList>
#include <QWidget>
#include <functional>
#include "FilterExpressions.h"
#include "FilterEllipse.h"

class FilterRowNode : public ICustomExpressionData
{
    QList<QWidget*> m_widgets;
    std::function<QLayout*()> m_layout = nullptr;
    
public:
    FilterRowNode(IExpression* exp, QList<QWidget*> widgets, std::function<QLayout*()> layout)
        : m_widgets(widgets), m_layout(layout)
    {
        m_exp = exp;
    }
    FilterRowNode(QList<QWidget*> widgets, std::function<QLayout*()> layout)
        : m_widgets(widgets), m_layout(layout)
    {
        
    }
    
    struct FillingFunctors
    {
        std::function<void(QWidget*)> addWidget;
        std::function<FilterEllipse*(IExpression*)> createEllipse;
        std::function<QWidget*(IExpression*)> createLBracket;
        std::function<QWidget*(IExpression*)> createRBracket;
        std::function<QWidget*(IExpression*)> createAndOp;
        std::function<QWidget*(IExpression*)> createOrOp;
        std::function<QLayout*()> getLayout;
    };
    
    
    static void CreateWidgets(IExpression* exp, FillingFunctors functors)
    {
        if(!exp)
            return;
        
        exp->Tree([functors](IExpression* exp)
                  {
                      FilterRowNode* data = nullptr;
                      if(auto fe = dynamic_cast<FilterExpression*>(exp))
                      {
                          auto ellipse = functors.createEllipse(exp);
                          {QSignalBlocker blocker{ellipse};
                              auto filterKey = fe->GetFilterKey();
                              auto filterOpKey = fe->GetFilterOperatorKey();
                              auto filtervalue = fe->GetFilterValue();
                              ellipse->selectFilter(filterKey);
                              ellipse->selectOperator(filterOpKey);
                              ellipse->SetValue(filtervalue);
                          }
                          data = new FilterRowNode({
                                                       ellipse
                                                   }, functors.getLayout);
                      }
                      else if(dynamic_cast<AndOperator*>(exp))
                      {
                          data = new FilterRowNode({
                                                       functors.createAndOp(exp)
                                                   }, functors.getLayout);
                      }
                      else if(dynamic_cast<OrOperator*>(exp))
                      {
                          data = new FilterRowNode({
                                                       functors.createOrOp(exp)
                                                   }, functors.getLayout);
                      }
                      else if(dynamic_cast<BracketsExpression*>(exp))
                      {
                          data = new FilterRowNode({
                                                       functors.createLBracket(exp),
                                                       functors.createRBracket(exp),
                                                   }, functors.getLayout);
                      }
                      
                      if(data && exp)
                          exp->SetData(data);
                  });
    }
    
    QList<QWidget*> FillWidgets()
    {
        QList<QWidget*> result;
        if(dynamic_cast<FilterExpression*>(m_exp))
        {
            result.append(m_widgets);
        }
        else if(auto binOp = dynamic_cast<IBinaryOperator*>(m_exp))
        {
            if(binOp->Left())
            {
                auto leftData = binOp->Left()->GetData<FilterRowNode>();
                if(leftData)
                    result.append(leftData->FillWidgets());
            }
            result.append(m_widgets);
            if(binOp->Right())
            {
                auto rightData = binOp->Right()->GetData<FilterRowNode>();
                if(rightData)
                    result.append(rightData->FillWidgets());
            }
        }
        else if(auto be = dynamic_cast<BracketsExpression*>(m_exp))
        {
            result.append(m_widgets[0]);
            if(be->GetChild())
            {
                auto childData = be->GetChild()->GetData<FilterRowNode>();
                if(childData)
                    result.append(childData->FillWidgets());
            }
            result.append(m_widgets[1]);
        }
        
        return result;
    }
    
    QList<QWidget*> GetWidgets()
    {
        return m_widgets;
    }
    
    void SetWidgets(QList<QWidget*> widgets)
    {
        m_widgets = widgets;
    }
    
    virtual ~FilterRowNode()
    {
        for(auto widget : m_widgets.toSet())
            if(widget)
            {
                if(m_layout)
                {
                    auto layout = m_layout();
                    if(layout)
                        layout->removeWidget(widget);
                }
                widget->deleteLater();
            }
    }
};



#endif // FILTERROWNODE_H
