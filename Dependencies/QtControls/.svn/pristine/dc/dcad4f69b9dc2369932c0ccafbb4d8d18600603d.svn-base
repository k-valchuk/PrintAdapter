#ifndef DATEFILTER_H
#define DATEFILTER_H

#include "IFilterElement.h"


class DateFilter : public IFilterElement
{
    QVariant m_key;
    QString m_name;
public:
    DateFilter(QVariant key, QString name);
    
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

#endif // DATEFILTER_H
