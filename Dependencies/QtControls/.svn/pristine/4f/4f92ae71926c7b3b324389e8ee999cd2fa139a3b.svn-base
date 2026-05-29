#include "FilterEllipse.h"
#include "qdebug.h"
#include "ui_FilterEllipse.h"

#include <QApplication>
#include "FiltersList.h"
#include "FilterRow.h"

void FilterEllipse::updateFilter(QSharedPointer<IFilterElement> oldFilter, QVariant oldOperator)
{
    ui->filter->setText(m_selectedElement->GetName());
    auto icon = m_selectedElement->GetIcon();
    ui->icon->setPixmap(icon.pixmap(25, 25));
    if(!icon.isNull())
        ui->icon->setContentsMargins(0, 0, 5, 0);
    else
        ui->icon->setContentsMargins(0, 0, 0, 0);
    for(const auto& op : m_selectedElement->GetOps())
        if(op.Key == m_selectedOperator)
        {
            ui->filterOperator->setText(op.Name);
            break;
        }
    
    for(int i = 0; i < ui->editorContainer->layout()->count(); i++)
    {
        auto child = ui->editorContainer->layout()->itemAt(i);
        if(child && child->widget() == m_editorWgt)
        {
            ui->editorContainer->layout()->removeItem(child);
            break;
        }
    }
    
    auto oldEditor = m_editorWgt;
    m_editorWgt = m_selectedElement->GetEditorWidget(m_selectedOperator, ui->editorContainer);
    m_valueLabel->setMinimumWidth(m_editorWgt ? 30 : 0);
    
    //если виджет редактора и тип фильтра такие же оставить значение
    if(m_editorWgt && oldEditor && typeid(*m_editorWgt) == typeid(*oldEditor) &&
        !m_selectedElement.isNull() && !oldFilter.isNull() && typeid(*m_selectedElement) == typeid(*oldFilter))
    {
        m_selectedElement->SetEditorValue(m_editorWgt, m_value, m_selectedOperator);
    }
    
    if(oldEditor)
    {
        m_blockOnFocusChange = true;
        delete oldEditor;
        m_blockOnFocusChange = false;
    }
    prepareEditorWgt();
    if(m_editorWgt)
        m_selectedElement->ConnectToEditingFinished(m_editorWgt, m_selectedOperator, this, [this]()
                                                {
                                                    shrink();
                                                });
    
    auto oldval = m_value;
    m_value = m_selectedElement->GetValueFromEditor(m_editorWgt, m_selectedOperator);
    m_valueLabel->setTextAndElide(getTextForValueLabel());
    if(oldval != m_value)
        emit valueChanged(this, m_selectedElement, m_selectedOperator, oldval, m_value);
    
    if(m_isExpanded && m_editorWgt)
    {
        ui->editorContainer->layout()->addWidget(m_editorWgt);
        ui->editorContainer->setFocusProxy(m_editorWgt);
    }
    else
    {
        if(m_isExpanded == false && m_editorWgt)
        {
            delete m_editorWgt;
            m_editorWgt = nullptr;
        }
        
        m_isExpanded = false;
    }
}

void FilterEllipse::selectFilter(QSharedPointer<IFilterElement> element)
{
    auto oldFilter = m_selectedElement;
    auto oldOperator = m_selectedOperator;
    m_selectedElement = element;
    m_selectedOperator = element->GetOps().first().Key;
    updateFilter(oldFilter, oldOperator);
    
    emit selectedFilterChanged(this, m_selectedElement, m_selectedOperator);
}

void FilterEllipse::selectFilter(QVariant filterKey)
{
    auto filters = m_parentFilter->GetFilters();
    if(filters.contains(filterKey))
        selectFilter(filters[filterKey]);
}

void FilterEllipse::selectOperator(QVariant opKey)
{
    auto oldFilter = m_selectedElement;
    auto oldOperator = m_selectedOperator;
    m_selectedOperator = opKey;
    updateFilter(oldFilter, oldOperator);
    
    emit selectedFilterChanged(this, m_selectedElement, m_selectedOperator);
}

void FilterEllipse::shrink()
{
    if(m_isExpanded == false)
        return;
    
    m_isExpanded = false;
    
    if(m_editorWgt)
        ui->editorContainer->layout()->removeWidget(m_editorWgt);
    auto oldval = m_value;
    m_value = m_selectedElement->GetValueFromEditor(m_editorWgt, m_selectedOperator);
    if(m_editorWgt)
        m_editorWgt->deleteLater();
    m_editorWgt = nullptr;
    ui->editorContainer->layout()->addWidget(m_valueLabel);
    m_valueLabel->setTextAndElide(getTextForValueLabel());
    m_valueLabel->show();
    
    emit valueChanged(this, m_selectedElement, m_selectedOperator, oldval, m_value);
}

void FilterEllipse::expand()
{
    if(m_isExpanded)
        return;
    
    m_editorWgt = m_selectedElement->GetEditorWidget(m_selectedOperator, ui->editorContainer);
    if(!m_editorWgt)
        return;
    
    m_isExpanded = true;
    
    m_selectedElement->ConnectToEditingFinished(m_editorWgt, m_selectedOperator, this, [this]()
                                                {
                                                    shrink();
                                                });
    m_selectedElement->SetEditorValue(m_editorWgt, m_value, m_selectedOperator);
    
    prepareEditorWgt();
    
    ui->editorContainer->layout()->removeWidget(m_valueLabel);
    m_valueLabel->hide();
    ui->editorContainer->layout()->addWidget(m_editorWgt);
    auto fproxy = m_selectedElement->GetFocusProxy(m_editorWgt, m_selectedOperator);
    ui->editorContainer->setFocusProxy(fproxy);
    this->setFocusProxy(fproxy);
    
    //заплатка для qt5.12, в 5.15 и без этого норм работает
    //без этого фокус не ставился по первому клику на редактор, видимо потому что фокус внутренне
    //изменялся уже до установки focusProxy строчками выше и они не приводили к желаемому результату
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    auto currentFocus = qApp->focusWidget();
    if(currentFocus == ui->editorContainer || currentFocus == this)
        m_editorWgt->setFocus();
#endif
    
    //m_editorWgt->setMinimumWidth(qMin(m_editorWgt->maximumWidth(), 180));
}

void FilterEllipse::prepareEditorWgt()
{
    if(!m_editorWgt)
        return;
    
    m_editorWgt->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    //m_editorWgt->setMaximumWidth(800);
    m_editorWgt->setStyleSheet("*"
                               "{"
                               "background-color: palette(mid);"
                               "background: palette(mid);"
                               "}");

//КУТЭ не всегда хочет обновлять стиль у детей в соответствии с родительским
//сначала думал что только в 5.12 и тут был ифдеф, но в 5.15 тоже иногда возникли проблемы и ифдеф убрал, пусть полишаются чо поделать
    auto children = m_editorWgt->findChildren<QWidget*>(QString());
    for(auto wgt : children)
        wgt->style()->polish(wgt);
}

QString FilterEllipse::getTextForValueLabel()
{
    return m_selectedElement->GetDisplayValue(m_value, m_selectedOperator);
}

FilterEllipse::FilterEllipse(FilterRow* parentFilter) :
    QFrame(parentFilter),
    m_parentFilter(parentFilter),
    ui(new Ui::FilterEllipse)
{
    ui->setupUi(this);
    this->setCursor(Qt::ArrowCursor);
    ui->editorContainer->setCursor(Qt::ArrowCursor);
    ui->icon->setCursor(Qt::ArrowCursor);
    
    m_valueLabel = ui->valueLabel;
    m_valueLabel->setMaximumWidth(500);
    
    ui->filter->setFocusProxy(this);
    ui->filterOperator->setFocusProxy(this);
    //ui->deleteElement->setFocusProxy(this);
    
    ui->icon->setFocusProxy(this);
    ui->valueLabel->setFocusProxy(this);
    auto font = ui->valueLabel->font();
    font.setBold(true);
    ui->valueLabel->setFont(font);
    
    connect(qApp, &QApplication::focusChanged, this, &FilterEllipse::focusChanged);
    
    auto filters = m_parentFilter->GetOrderedFilters();
    if(filters.size() > 0)
        selectFilter(filters.first());
    
    if(m_parentFilter && m_parentFilter->Config().OnlyEllipsesVisible)
    {
        ui->deleteElement->hide();
        ui->horizontalSpacer_4->changeSize(10, 1);
    }
}

void FilterEllipse::focusChanged(QWidget* old, QWidget* now)
{
    if(m_blockOnFocusChange)
        return;
    
    if(now == m_editorWgt)
        ui->deleteElement->setFocusProxy(m_editorWgt);
    else
        ui->deleteElement->setFocusProxy(nullptr);
    
    if(now == ui->deleteElement)
        return;
    
    if(now && (now == this || now == m_editorWgt || now == ui->editorContainer ||
        (now && now->window() == m_filtersList) ||
        (now && now->window() == m_opsList) ||
        (now && m_editorWgt && !m_selectedElement.isNull() && m_selectedElement->IsPartOfEditorWidget(m_editorWgt, now)) 
                ))
        expand();
    else
        shrink();
}

FilterEllipse::~FilterEllipse()
{
    disconnect(qApp, &QApplication::focusChanged, this, &FilterEllipse::focusChanged);
    if(m_editorWgt)
        delete m_editorWgt;
    delete ui;
    
    if(m_filtersList)
        delete m_filtersList;
    if(m_opsList)
        delete m_opsList;
}

void FilterEllipse::SetColor(QColor color)
{
    auto stylesheet = this->styleSheet();
    stylesheet = stylesheet.replace("green", QString("rgb(%1, %2, %3)")
                                                 .arg(color.red())
                                                 .arg(color.green())
                                                 .arg(color.blue()));
    this->setStyleSheet(stylesheet);
}

bool FilterEllipse::IsAllFieldsFilter()
{
    auto allowedOpsForFilterToBeAll = m_parentFilter->AllFieldsAllowedOperators();
    if(!m_selectedElement.isNull() && m_selectedElement == m_parentFilter->AllFieldsFielter()
        && (allowedOpsForFilterToBeAll.size() == 0 || allowedOpsForFilterToBeAll.contains(m_selectedOperator)) )
        return true;
    
    return false;
}

QVariant FilterEllipse::GetValue()
{
    return m_value;
}

QString FilterEllipse::GetDisplayValue()
{
    return m_selectedElement->GetDisplayValue(m_value, m_selectedOperator);
}

void FilterEllipse::SetValue(QVariant value)
{
    auto old = m_value;
    m_value = value;
    if(m_isExpanded && m_editorWgt)
        m_selectedElement->SetEditorValue(m_editorWgt, value, m_selectedOperator);
    
    m_valueLabel->setTextAndElide(getTextForValueLabel());
    
    emit valueChanged(this, m_selectedElement, m_selectedOperator, old, m_value);
}

QVariant FilterEllipse::GetFilterExpression()
{
    return m_selectedElement->GetFilterExpression(m_selectedOperator, m_value);
}

bool FilterEllipse::IsExpanded()
{
    return m_isExpanded;
}

void FilterEllipse::moveEvent(QMoveEvent* event)
{
    auto buttonPos = ui->filter->geometry();
    auto pos = buttonPos.bottomLeft();
    pos.setY(pos.y() + 10);
    if(m_filtersList)
        m_filtersList->move(this->mapToGlobal(pos));
    QFrame::moveEvent(event);
}

void FilterEllipse::filtersListClicked()
{
    if(m_parentFilter->GetFilters().size() <= 1)
        return;
    
    if(m_filtersList == nullptr)
    {
        m_filtersList = new FiltersList(m_parentFilter);
        //m_filtersList->setWindowFlag(Qt::Window, true);
        //m_filtersList->setWindowFlag(Qt::FramelessWindowHint, true);
        m_filtersList->setWindowFlag(Qt::Popup, true);
        
        connect(m_filtersList, &FiltersList::filterSelected, this, [this](QVariant filterKey)
                {
                    for(auto filter : m_parentFilter->GetFilters().values())
                        if(filter->GetElementId() == filterKey)
                        {
                            selectFilter(filter);
                            if(m_editorWgt)
                                m_editorWgt->setFocus();
                            break;
                        }
                });
    }
    
    if(!m_filtersList->isVisible())
    {
        m_filtersList->show();
        m_filtersList->raise();
        m_filtersList->SetFocusToSearch();
    }
    
    auto buttonPos = ui->filter->geometry();
    auto pos = buttonPos.bottomLeft();
    pos.setY(pos.y() + 14);
    m_filtersList->move(this->mapToGlobal(pos));
}

void FilterEllipse::opsListClicked()
{
    if(m_selectedElement->GetOps().size() <= 1)
        return;
    
    if(m_opsList == nullptr)
    {
        m_opsList = new OperatorList();
        //m_filtersList->setWindowFlag(Qt::Window, true);
        //m_filtersList->setWindowFlag(Qt::FramelessWindowHint, true);
        m_opsList->setWindowFlag(Qt::Popup, true);
        
        connect(m_opsList, &OperatorList::operatorSelected, this, [this](QVariant opKey)
                {
                    selectOperator(opKey);
                    if(m_editorWgt)
                        m_editorWgt->setFocus();
                });
    }
    
    m_opsList->SetOpsList(m_selectedElement->GetOps());
    
    if(!m_opsList->isVisible())
    {
        m_opsList->show();
        m_opsList->raise();
    }
    
    auto buttonPos = ui->filterOperator->geometry();
    auto pos = buttonPos.bottomLeft();
    pos.setY(pos.y() + 14);
    m_opsList->move(this->mapToGlobal(pos));
}

void FilterEllipse::deleteElementClicked()
{
    emit elementDeleted(this);
}
