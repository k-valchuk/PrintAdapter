#include "TextFilter.h"
#include <QApplication>
#include <QLineEdit>

TextFilter::TextFilter(QVariant key, QString name)
{
    Q_INIT_RESOURCE(filter);
    m_key = key;
    m_name = name;
}

QIcon TextFilter::GetIcon()
{
    return QIcon(":/filter/text_filter");
}

QList<FilterOperator> TextFilter::GetOps()
{
    return QList<FilterOperator>
    {
        FilterOperator{"contains", QApplication::tr("Contains")},
        FilterOperator{"notContains", QApplication::tr("Not contains")}
    };
}

QWidget* TextFilter::GetEditorWidget(QVariant operatorKey, QWidget* parent)
{
    auto lineEdit = new QLineEdit();
    return lineEdit;
}

QVariant TextFilter::GetFilterExpression(QVariant operatorKey, QVariant value)
{
    return QString("%1 %2").arg(operatorKey.toString()).arg(value.toString());
}


QVariant TextFilter::GetElementId()
{
    return m_key;
}

QString TextFilter::GetName()
{
    return m_name;
}


QVariant TextFilter::GetValueFromEditor(QWidget* editor, QVariant operatorKey)
{
    if(editor)
        return ((QLineEdit*)editor)->text();
    else
        return QString();
}

void TextFilter::SetEditorValue(QWidget* editor, QVariant value, QVariant operatorKey)
{
    if(editor)
        ((QLineEdit*)editor)->setText(value.toString());
}


QString TextFilter::GetDisplayValue(QVariant value, QVariant operatorKey)
{
    return value.toString();
}

void TextFilter::ConnectToEditingFinished(QWidget* editor, QVariant opKey, QObject* receiver, std::function<void()> functor)
{
    QObject::connect((QLineEdit*)editor, &QLineEdit::editingFinished, receiver, functor);
}
