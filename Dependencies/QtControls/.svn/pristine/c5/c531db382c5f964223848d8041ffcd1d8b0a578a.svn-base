
#ifndef FILTERROWTOOLTIP_H
#define FILTERROWTOOLTIP_H

#include "FilterExpressions.h"
#include "IFilterElement.h"
#include <QList>
#include <QVector>

class FilterRowToolTip
{
    std::function<QSharedPointer<IExpression>()> m_expGetter = nullptr;
    std::function<QSharedPointer<IFilterElement>()> m_allFiltGetter = nullptr;
    std::function<QList<QSharedPointer<IFilterElement>>()> m_filtsGetter = nullptr;
public:
    void SetExpGetter(std::function<QSharedPointer<IExpression>()> getter,
                      std::function<QSharedPointer<IFilterElement>()> allFiltGetter,
                      std::function<QList<QSharedPointer<IFilterElement>>()> filtsGetter);
    void ShowTooltip(QPoint pos, QWidget* parent);
};

#endif // FILTERROWTOOLTIP_H
