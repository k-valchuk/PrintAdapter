#include "ComboFilter.h"
#include "qabstractitemview.h"
#include <QApplication>
#include <QComboBox>

ComboFilter::ComboFilter(QVariant key, QString name, QList<QPair<QVariant, QString>> data)
{
    Q_INIT_RESOURCE(filter);
    m_key = key;
    m_name = name;
    m_data = data;
    for(const auto& item : m_data)
        m_dataMap.insert(item.first, item.second);
}

QIcon ComboFilter::GetIcon()
{
    return QIcon(":/filter/text_filter");
}

QList<FilterOperator> ComboFilter::GetOps()
{
    return QList<FilterOperator>
    {
        FilterOperator{"equal", QApplication::tr("=")},
        FilterOperator{"notEqual", QApplication::tr("!=")}
    };
}

QWidget* ComboFilter::GetEditorWidget(QVariant operatorKey, QWidget* parent)
{
    auto cmb = new QComboBox();
    //cmb->view()->setFocusProxy(cmb); //use this or override IsPartOfEditorWidget to return true if some wgt is popup child of editor
    for(const auto& item : m_data)
        cmb->addItem(item.second, item.first);
    
    return cmb;
}

QVariant ComboFilter::GetFilterExpression(QVariant operatorKey, QVariant value)
{
    return QString("%1 %2").arg(operatorKey.toString()).arg(value.toString());
}


QVariant ComboFilter::GetElementId()
{
    return m_key;
}

QString ComboFilter::GetName()
{
    return m_name;
}


QVariant ComboFilter::GetValueFromEditor(QWidget* editor, QVariant operatorKey)
{
    if(editor)
        return ((QComboBox*)editor)->currentData();
    else
        return QVariant();
}

void ComboFilter::SetEditorValue(QWidget* editor, QVariant value, QVariant operatorKey)
{
    if(editor)   
    {
        auto cmb =((QComboBox*)editor);
        for(int i = 0; i < cmb->count(); i++)
            if(cmb->itemData(i) == value)
            {
                cmb->setCurrentIndex(i);
                break;
            }
    }
}


QString ComboFilter::GetDisplayValue(QVariant value, QVariant operatorKey)
{
    if(m_dataMap.contains(value))
        return m_dataMap[value];
    else
        return QString();
}

void ComboFilter::ConnectToEditingFinished(QWidget* editor, QVariant opKey, QObject* receiver, std::function<void()> functor)
{
    //QObject::connect((QComboBox*)editor, &QComboBox::currentTextChanged, receiver, functor);
}


bool ComboFilter::IsPartOfEditorWidget(QWidget* editor, QWidget* tested)
{
    if(tested == ((QComboBox*)editor)->view())
        return true;
    
    return false;
}
