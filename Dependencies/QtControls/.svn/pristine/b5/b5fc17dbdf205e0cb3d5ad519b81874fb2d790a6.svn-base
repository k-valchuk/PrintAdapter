#ifndef IFILTERELEMENT_H
#define IFILTERELEMENT_H

#include <QWidget>
#include <QVariant>
#include <QIcon>
#include <QJsonObject>

struct FilterOperator
{
    QVariant Key;
    QString Name;
};

class IFilterElement
{
public:
    
    virtual QIcon GetIcon() = 0;
    virtual QList<FilterOperator> GetOps() = 0;
    virtual QWidget* GetEditorWidget(QVariant operatorKey, QWidget* parent) = 0;
    virtual QVariant GetFilterExpression(QVariant operatorKey, QVariant value) = 0;
    
    virtual QVariant GetElementId() = 0;
    virtual QString GetName() = 0;
    
    virtual QVariant GetValueFromEditor(QWidget* editor, QVariant operatorKey) = 0;
    virtual QString GetDisplayValue(QVariant value, QVariant operatorKey) = 0;
    virtual void SetEditorValue(QWidget* editor, QVariant value, QVariant operatorKey) = 0;
    virtual void ConnectToEditingFinished(QWidget* editor, QVariant opKey, QObject* receiver, std::function<void()> functor) {}
    
    virtual bool IsPartOfEditorWidget(QWidget* editor, QWidget* tested) { return false; };
    
    virtual QWidget* GetFocusProxy(QWidget* editor, QVariant operatorKey) { return editor; }
};

#endif // IFILTERELEMENT_H
