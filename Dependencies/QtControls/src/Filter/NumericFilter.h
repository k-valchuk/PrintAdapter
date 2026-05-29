#ifndef NUMERICFILTER_H
#define NUMERICFILTER_H

#include "IFilterElement.h"


class NumericFilter : public IFilterElement
{
    QVariant m_key;
    QString m_name;
    
    int m_min, m_max;
public:
    NumericFilter(QVariant key, QString name, int min = 1, int max = 2147483647);
    
    QIcon GetIcon() override;
    QList<FilterOperator> GetOps() override;
    QWidget* GetEditorWidget(QVariant operatorKey, QWidget* parent) override;
    QVariant GetFilterExpression(QVariant operatorKey, QVariant value) override;

    QVariant GetElementId() override;
    QString GetName() override;

    QVariant GetValueFromEditor(QWidget* editor, QVariant operatorKey) override;
    void SetEditorValue(QWidget* editor, QVariant value, QVariant operatorKey) override;
    QString GetDisplayValue(QVariant value, QVariant operatorKey) override;
    void ConnectToEditingFinished(QWidget* editor, QVariant opKey, QObject* receiver, std::function<void()> functor) override;
};

#endif // TEXTFILTER_H
