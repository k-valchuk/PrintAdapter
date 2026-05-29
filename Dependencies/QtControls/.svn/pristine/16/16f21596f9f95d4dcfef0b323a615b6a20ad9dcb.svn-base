#include "DateFilter.h"
#include <QApplication>
#include <QDateEdit>
#include <QDateTimeEdit>

DateFilter::DateFilter(QVariant key, QString name)
{
    Q_INIT_RESOURCE(filter);
    m_key = key;
    m_name = name;
}

QIcon DateFilter::GetIcon()
{
    return QIcon(":/filter/calendar_filter");
}

QList<FilterOperator> DateFilter::GetOps()
{
    return QList<FilterOperator>
    {
        FilterOperator{"from", QApplication::tr("Starting from")},
        FilterOperator{"ends", QApplication::tr("Ending at")}
    };
}

QWidget* DateFilter::GetEditorWidget(QVariant operatorKey, QWidget* parent)
{
    auto edit = new QDateTimeEdit();
    edit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    edit->setDateTime(QDateTime::currentDateTime());
    return edit;
}

QVariant DateFilter::GetFilterExpression(QVariant operatorKey, QVariant value)
{
    return QString("%1 %2").arg(operatorKey.toString()).arg(value.toString());
}


QVariant DateFilter::GetElementId()
{
    return m_key;
}

QString DateFilter::GetName()
{
    return m_name;
}


QVariant DateFilter::GetValueFromEditor(QWidget* editor, QVariant operatorKey)
{
    return ((QDateTimeEdit*)editor)->dateTime().toUTC().toString(Qt::ISODate);
}

void DateFilter::SetEditorValue(QWidget* editor, QVariant value, QVariant operatorKey)
{
    ((QDateTimeEdit*)editor)->setDateTime(QDateTime::fromString(value.toString(), Qt::ISODate).toLocalTime());
}


QString DateFilter::GetDisplayValue(QVariant value, QVariant operatorKey)
{
    auto date = QDateTime::fromString(value.toString(), Qt::ISODate).toLocalTime();
    return date.toString("dd.MM.yyyy HH:mm");
}

void DateFilter::ConnectToEditingFinished(QWidget* editor, QVariant opKey, QObject* receiver, std::function<void()> functor)
{
    QObject::connect((QDateTimeEdit*)editor, &QDateEdit::editingFinished, receiver, functor);
}
