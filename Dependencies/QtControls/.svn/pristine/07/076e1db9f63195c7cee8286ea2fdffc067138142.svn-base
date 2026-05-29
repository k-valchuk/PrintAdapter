#include "NumericFilter.h"
#include "qvalidator.h"
#include <QApplication>
#include <QLineEdit>

NumericFilter::NumericFilter(QVariant key, QString name, int min, int max)
{
    Q_INIT_RESOURCE(filter);
    m_key = key;
    m_name = name;
    m_min = min;
    m_max = max;
}

QIcon NumericFilter::GetIcon()
{
    return QIcon(":/filter/text_filter");
}

QList<FilterOperator> NumericFilter::GetOps()
{
    return QList<FilterOperator>
    {
        FilterOperator{"equal", "="},
        FilterOperator{"notEqual", "!="},
        FilterOperator{"more", ">"},
        FilterOperator{"less", "<"}
    };
}

QWidget* NumericFilter::GetEditorWidget(QVariant operatorKey, QWidget* parent)
{
    auto lineEdit = new QLineEdit();
    auto validator = new QIntValidator();
    validator->setRange(m_min, m_max);
    lineEdit->setValidator(validator);
    return lineEdit;
}

QVariant NumericFilter::GetFilterExpression(QVariant operatorKey, QVariant value)
{
    return QString("%1 %2").arg(operatorKey.toString()).arg(value.toString());
}


QVariant NumericFilter::GetElementId()
{
    return m_key;
}

QString NumericFilter::GetName()
{
    return m_name;
}


QVariant NumericFilter::GetValueFromEditor(QWidget* editor, QVariant operatorKey)
{
    return ((QLineEdit*)editor)->text().toInt();
}

void NumericFilter::SetEditorValue(QWidget* editor, QVariant value, QVariant operatorKey)
{
    ((QLineEdit*)editor)->setText(QString("%1").arg(value.toInt()));
}


QString NumericFilter::GetDisplayValue(QVariant value, QVariant operatorKey)
{
    return QString("%1").arg(value.toInt());
}

void NumericFilter::ConnectToEditingFinished(QWidget* editor, QVariant opKey, QObject* receiver, std::function<void()> functor)
{
    QObject::connect((QLineEdit*)editor, &QLineEdit::editingFinished, receiver, functor);
}
