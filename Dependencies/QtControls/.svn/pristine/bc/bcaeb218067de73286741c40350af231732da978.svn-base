#ifndef COMBOFILTER_H
#define COMBOFILTER_H

#include "IFilterElement.h"


class ComboFilter : public IFilterElement
{
    QVariant m_key;
    QString m_name;
    
protected:
    QList<QPair<QVariant, QString>> m_data;
    QMap<QVariant, QString> m_dataMap;
public:
    ComboFilter(QVariant key, QString name, QList<QPair<QVariant, QString>> data);
    
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
    
    bool IsPartOfEditorWidget(QWidget* editor, QWidget* tested) override;
};

#endif // COMBOFILTER_H
