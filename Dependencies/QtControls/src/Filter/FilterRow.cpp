#include "FilterRow.h"
#include "qjsondocument.h"
#include "qmenu.h"
#include "ui_FilterRow.h"
#include <QLineEdit>
#include "FilterRowNode.h"

//#define DEBUG_FILTER_JSON

void FilterRow::toLineEdit()
{
    if(m_config.AddableLineEdit == false)
    {
        if(m_lineEditLayout)
            return;
        
        auto ellipses = EllipseWidgetsInRow();
        if(ellipses.size() == 1)
        {
            auto ellipse = ellipses.first();
            if(ellipse->IsAllFieldsFilter())
                m_lineEdit->setText(ellipse->GetDisplayValue());
        }
        
        m_lineEditLayout = new QHBoxLayout();
        m_lineEditLayout->setContentsMargins(0, 0, 0, 0);
        m_lineEditLayout->setSpacing(0);
        
        m_lineEditLayout->addWidget(m_lineEdit);
        
        m_rowLayoutItems.clear();
        while(m_elementsContainer->count() > 0)
        {
            auto item = m_elementsContainer->takeAt(0);
            if(item->widget())
            {
                item->widget()->hide();
                m_rowLayoutItems.append(item->widget());
            }
            delete item;
        }
        
        delete m_elementsContainer;
        m_elementsContainer = nullptr;
        
        ui->frame->setLayout(m_lineEditLayout);
        m_lineEdit->show();
        m_lineEdit->setFocus();
    }
    else
    {
        if(m_lineEdit->isVisible())
            return;
        
        ui->addElement->hide();
        
        auto ellipses = EllipseWidgetsInRow();
        if(ellipses.size() > 0 && ellipses.constLast()->GetDisplayValue().isEmpty())
        {
            m_deletedBeforeLineEditFilterType = ellipses.constLast()->selectedFilter();
            m_deletedBeforeLineEditFilterOp = ellipses.constLast()->selectedOperator();
            
            if(m_widgetToExp.contains(ellipses.last()))
            {
                auto node = m_widgetToExp[ellipses.last()];
                bool isFirstLevel = true;
                auto exp = m_widgetToExp[ellipses.last()];
                
                auto parentTest = exp->Parent();
                while(parentTest && parentTest != &m_root)
                {
                    if(dynamic_cast<BracketsExpression*>(parentTest))
                    {
                        isFirstLevel = false;
                        break;
                    }
                    parentTest = parentTest->Parent();
                }
                if(isFirstLevel)
                    removeFilterElement(ellipses.last());
            }
        }
        
        m_elementsContainer->addWidget(m_lineEdit);
        m_lineEdit->show();
        m_lineEdit->setFocus();
    }
}

void FilterRow::toEllipsesRow()
{
    if(m_config.AddableLineEdit == false)
    {
        if(m_lineEditLayout == nullptr)
            return;
        
        delete m_lineEditLayout->takeAt(0);
        delete m_lineEditLayout;
        m_lineEditLayout = nullptr;
        
        m_lineEdit->hide();
        
        m_elementsContainer = new FlowLayout(4, 4, 4, m_config.OnlyEllipsesVisible ? 0 : 50);
        ui->frame->setLayout(m_elementsContainer);
        for(auto item : m_rowLayoutItems)
        {
            if(item)
                item->show();
            m_elementsContainer->addWidget(item);
        }
        
        auto text = m_lineEdit->text();
        m_lineEdit->clear();
        
        auto ellipses = EllipseWidgetsInRow();
        if(ellipses.size() == 0)
        {
            if(text.isEmpty() == false)
            {
                QSignalBlocker blocker{this};
                auto element = addFilterElement();
                if(element)
                {
                    QSignalBlocker elBlocker{element};
                    element->selectFilter(m_allFieldsFilter);
                    elBlocker.unblock();
                    blocker.unblock();
                    element->SetValue(text);
                }
            }
        }
        else if(ellipses.size() == 1)
        {
            auto ellipse = ellipses.first();
            QSignalBlocker elBlocker{ellipse};
            ellipse->selectFilter(m_allFieldsFilter);
            if(m_allFieldsAllowedOps.size() > 0)
                ellipse->selectOperator(m_allFieldsAllowedOps.first());
            elBlocker.unblock();
            
            if(text.isEmpty() == false)
                ellipse->SetValue(text);
            else
                ellipse->Delete();
        }
    }
    else
    {
        if(m_lineEdit->isVisible() == false)
            return;
        
        m_lineEdit->hide();
        auto text = m_lineEdit->text();
        m_lineEdit->clear();
        m_elementsContainer->removeWidget(m_lineEdit);
        ui->addElement->show();
        ui->addElement->setFocus();
        
        if(!text.isEmpty())
        {
            QSignalBlocker blocker{this};
            auto element = addFilterElement();
            QSignalBlocker elBlocker{element};
            if(m_deletedBeforeLineEditFilterType.isNull())
            {
                element->selectFilter(m_allFieldsFilter);
                if(m_allFieldsAllowedOps.size() > 0)
                    element->selectOperator(m_allFieldsAllowedOps.first());
            }
            else
            {
                element->selectFilter(m_deletedBeforeLineEditFilterType);
                element->selectOperator(m_deletedBeforeLineEditFilterOp);
                m_deletedBeforeLineEditFilterType = nullptr;
            }
            blocker.unblock();
            elBlocker.unblock();
            
            element->SetValue(text);
        }
    }
}

QList<FilterEllipse*> FilterRow::EllipseWidgetsInRow()
{
    if(!m_elementsContainer)
        return {};
    
    QList<FilterEllipse*> result;
    
    for(int i = 0; i < m_elementsContainer->count(); i++)
    {
        auto item = m_elementsContainer->itemAt(i);
        if(item)
        {
            auto wgt = item->widget();
            if(wgt)
                if(auto el = dynamic_cast<FilterEllipse*>(wgt))
                    result.append(el);
        }
    }
    return result;
}

bool FilterRow::isToLineEditAllowed()
{
    if(m_config.AddableLineEdit)
        return true;
    
    if(!m_elementsContainer)
        return false;
    
    auto ellipses = EllipseWidgetsInRow();
    bool onlyButton = ellipses.size() == 0;
    bool isAllFieldsFilter = false;
    if(ellipses.size() == 1)
    {
        auto ellipse = ellipses.first();
        if(ellipse->IsAllFieldsFilter())
            isAllFieldsFilter = true;
    }
    
    return onlyButton | isAllFieldsFilter;
}

void FilterRow::InitLineEdit()
{
    m_lineEdit = new QLineEdit();
    m_lineEdit->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    m_lineEdit->setStyleSheet(QString(
                              "QLineEdit{"
                              "border-radius: 15px;"
                              "border: none;"
                              "background-color: palette(mid);"
                              "padding-left: 15px;"
                              "%1"
                              "}")
                                  .arg(m_config.AddableLineEdit ? "margin-top: 4px;" : ""));
    m_lineEdit->hide();
    
    ui->frame->setFocusPolicy(Qt::StrongFocus);
    connect(qApp, &QApplication::focusChanged, this, &FilterRow::onFocusChanged);
    connect(m_lineEdit, &QLineEdit::returnPressed, this, [this](){
        toEllipsesRow();
        if(m_config.AddableLineEdit)
            toLineEdit();
        if(m_searchOnEnter)
            emit searchClicked();
    });
    
    if(m_lineEdit && m_elementsContainer && isToLineEditAllowed())
        ui->frame->setCursor(Qt::IBeamCursor);
    else
        ui->frame->setCursor(Qt::ArrowCursor);
}

void FilterRow::emitFilterChanged()
{
    if(m_config.IsTogglable)
    {
        if(m_toggled)
        {
            auto newExp = m_root.CopyOfChild();
            if(newExp || m_wasNull == false)
                emit filterChanged(QSharedPointer<IExpression>(newExp));
            if(newExp)
                m_wasNull = false;
        }
        else
        {
            if(m_wasNull == false)
                emit filterChanged(nullptr);
            m_wasNull = true;
        }
    }
    else
        emit filterChanged(QSharedPointer<IExpression>(m_root.CopyOfChild()));
}


void FilterRow::setup()
{
    m_elementsContainer = new FlowLayout(4, 0, 4, m_config.OnlyEllipsesVisible ? 0 : 50);
    m_elementsContainer->SetConsiderInvisibleWidgets(true);
    
    ui->filterRowLayout->removeWidget(ui->addElement);
    ui->frame->setLayout(m_elementsContainer);
    m_elementsContainer->addWidget(ui->addElement);
    //m_elementsContainer->setAlignment(Qt::AlignCenter);
    
    ui->addElement->setCursor(Qt::ArrowCursor);
    
    if(m_allFieldsFilter.isNull() == false)
        InitLineEdit();
    
    m_colors.append(QColor::fromRgb(42, 108, 165));
    m_colors.append(QColor::fromRgb(107, 138, 23));
    m_colors.append(QColor::fromRgb(170, 120, 5));
    m_colors.append(QColor::fromRgb(138, 23, 96));
    m_colors.append(QColor::fromRgb(35, 148, 130));
    m_colors.append(QColor::fromRgb(142, 38, 38));
    
    connect(this, &FilterRow::treeChanged, this, &FilterRow::debugPrintTree);
    
    m_fillingFunctors.addWidget = [this](QWidget* wgt)
    {
        this->m_elementsContainer->addWidget(wgt);
    };
    m_fillingFunctors.createAndOp = [this](IExpression* exp)
    {
        auto btn = this->CreateOperatorButton("&&");
        this->m_btnToExp[btn] = exp;
        return btn;
    };
    m_fillingFunctors.createOrOp = [this](IExpression* exp)
    {
        auto btn = this->CreateOperatorButton("|");
        this->m_btnToExp[btn] = exp;
        return btn;
    };
    m_fillingFunctors.createLBracket = [this](IExpression* exp)
    {
        auto wgt = new BracketLabel("(");
        return wgt;
    };
    m_fillingFunctors.createRBracket = [this](IExpression* exp)
    {
        auto wgt = new BracketLabel(")");
        return wgt;
    };
    m_fillingFunctors.getLayout = [this]()
    {
        return m_elementsContainer;
    };
    m_fillingFunctors.createEllipse = [this](IExpression* exp)
    {
        auto el = this->CreateFilterEllipse();
        this->m_widgetToExp[el] = dynamic_cast<FilterExpression*>(exp);
        return el;
    };
    
    if(m_config.OnlyEllipsesVisible)
    {
        ui->addElement->hide();
        ui->clear->hide();
        ui->enter->setEnabled(false);
        ui->filterRowLayout->removeItem(ui->horizontalSpacer);
        ui->filterRowLayout->removeItem(ui->horizontalSpacer_2);
    }
}

FilterRow::FilterRow(QWidget* parent)
    : QFrame(parent),
    ui(new Ui::FilterRow),
    m_config{true, true}
{
    ui->setupUi(this);
    setup();
}

FilterRow::FilterRow(FilterRowConfig config, QWidget* parent)
    : QFrame(parent),
    ui(new Ui::FilterRow),
    m_config(config)
{
    ui->setupUi(this);
    setup();
}

FilterRow::FilterRow(QList<QSharedPointer<IFilterElement>> filters, QSharedPointer<IFilterElement> allFieldsFilter,
                     FilterRowConfig config, QWidget *parent) :
    QFrame(parent), m_allFieldsFilter(allFieldsFilter),
    ui(new Ui::FilterRow), m_config(config)
{
    ui->setupUi(this);
    
    m_orderedFilters = filters;
    for(auto filter : filters)
    {
        m_filters[filter->GetElementId()] = filter;
        m_favouriteFilters[filter->GetElementId()] = false;
    }
    
    setup();
}

FilterRow::~FilterRow()
{
    disconnect(qApp, &QApplication::focusChanged, this, &FilterRow::onFocusChanged);
    delete ui;
}

void FilterRow::SetAvailableFiltersList(QList<QSharedPointer<IFilterElement>> filters)
{
    m_filters.clear();
    m_favouriteFilters.clear();
    
    m_orderedFilters = filters;
    for(auto filter : filters)
    {
        m_filters[filter->GetElementId()] = filter;
        m_favouriteFilters[filter->GetElementId()] = false;
    }
    
    QSet<FilterEllipse*> nodesToDelete;
    m_root.Tree([&](IExpression* node)
                                {
                                    QList<QWidget*> wgts;
                                    if(node->GetData<FilterRowNode>())
                                    wgts.append(node->GetData<FilterRowNode>()->GetWidgets());
                                    if(wgts.size() == 0)
                                        return;
                                    for(auto wgt : wgts)
                                    if(auto ellipse = dynamic_cast<FilterEllipse*>(wgt))
                                    {
                                        auto filter = ellipse->selectedFilter();
                                        auto op = ellipse->selectedOperator();
                                        if(!m_filters.contains(filter->GetElementId()))
                                        {
                                            nodesToDelete.insert(ellipse);
                                            return;
                                        }
                                        else
                                        {
                                            QList<QVariant> newOps;
                                            for(auto oper : m_filters[filter->GetElementId()]->GetOps())
                                                newOps.append(oper.Key);
                                            if(!newOps.contains(op))
                                            {
                                                nodesToDelete.insert(ellipse);
                                                return;
                                            }
                                            
                                            auto oldValue = ellipse->GetValue();
                                            ellipse->selectFilter(m_filters[filter->GetElementId()]);
                                            ellipse->selectOperator(op);
                                            ellipse->SetValue(oldValue);
                                        }
                                    }
                                });
    for(auto node : nodesToDelete)
        if(node)
            removeFilterElement(node);
}

void FilterRow::SetAllFieldsFilter(QSharedPointer<IFilterElement> allFieldsFilter)
{
    auto old = m_allFieldsFilter;
    m_allFieldsFilter = allFieldsFilter;
    if(old.isNull())
        InitLineEdit();
}

void FilterRow::SetAllFieldsAllowedOps(QVector<QVariant> allFieldsAllowedOps)
{
    m_allFieldsAllowedOps = allFieldsAllowedOps;
    
    if(m_lineEdit && m_elementsContainer && isToLineEditAllowed())
        ui->frame->setCursor(Qt::IBeamCursor);
    else
        ui->frame->setCursor(Qt::ArrowCursor);
}

int FilterRow::SizeOfFilters()
{
    if(!m_elementsContainer)
        return 0;
    
    int size = 0;
    for(int i = 0; i < m_elementsContainer->count(); i++)
    {
        auto item = m_elementsContainer->itemAt(i);
        if(item)
        {
            auto wgt = item->widget();
            if(wgt && !wgt->isHidden())
                size += wgt->sizeHint().width() + m_elementsContainer->spacing();
        }
    }
    size += m_elementsContainer->contentsMargins().left() + m_elementsContainer->contentsMargins().right();
    return size;
}

int FilterRow::WholeSize()
{
    auto otherWidth = this->sizeHint().width() - ui->frame->sizeHint().width();
    return SizeOfFilters() + otherWidth;
}

int FilterRow::GetHeightForWidth(int width)
{
    if(m_elementsContainer)
        return m_elementsContainer->heightForKnownWidth(width)
               + ui->frame->contentsMargins().top()
               + ui->frame->contentsMargins().bottom() + 2;
    return 0;
}

void FilterRow::AddCustomButton(QWidget* button)
{
    ui->buttonsLayout->insertWidget(ui->buttonsLayout->count()-1, button);
}

void FilterRow::RemoveCustomButton(QWidget* button)
{
    ui->buttonsLayout->removeWidget(button);
}

void FilterRow::SetSearchButtonVisible(bool visible)
{
    ui->enter->setVisible(visible);
}

void FilterRow::restoreFromJson(QJsonObject obj)
{
    restoreFromTree(IExpression::FromJson(obj));
}

void FilterRow::restoreFromTree(IExpression* exp)
{
    {QSignalBlocker b{this};
        clearFilter();
    }
    
    this->toEllipsesRow();
    m_colorIndex = 0;
    
    if(!exp)
        return;
    
    auto btn = m_elementsContainer->takeAt(0);
    
    auto newExpTree = exp;
    m_root.SetChild(newExpTree);
    if(newExpTree)
        newExpTree->SetParent(&m_root);
    
    if(newExpTree)
    {
        FilterRowNode::CreateWidgets(newExpTree, m_fillingFunctors);
        auto wgts = newExpTree->GetData<FilterRowNode>()->FillWidgets();
        
        for(auto wgt : wgts)
            m_elementsContainer->addWidget(wgt);
    }
    
    m_elementsContainer->addItem(btn);
    
    emitFilterChanged();
    emit treeChanged();
}

void FilterRow::onFocusChanged(QWidget* old, QWidget* now)
{
    if(now && (
            now == ui->frame ||
            now == m_lineEdit
            ) && (m_elementsContainer && isToLineEditAllowed() || !m_elementsContainer))
        toLineEdit();
    else
        toEllipsesRow();
}

FilterEllipse* FilterRow::CreateFilterEllipse()
{
    auto element = new FilterEllipse(this);
    
    if(m_colors.size() > 0)
        element->SetColor(m_colors[m_colorIndex++ % m_colors.size()]);
    
    connect(element, &FilterEllipse::elementDeleted, this, &FilterRow::removeFilterElement);
    connect(element, &FilterEllipse::selectedFilterChanged, this, [this](FilterEllipse* element, QSharedPointer<IFilterElement> selected, QVariant selectedOp)
            {
                if(m_widgetToExp.contains(element))
                    if(auto exp = dynamic_cast<FilterExpression*>(m_widgetToExp[element]))
                        exp->SetFilter(selected->GetElementId(), selectedOp);
                
                if(m_lineEdit && m_elementsContainer && isToLineEditAllowed())
                    ui->frame->setCursor(Qt::IBeamCursor);
                else
                    ui->frame->setCursor(Qt::ArrowCursor);
                
                emitFilterChanged();
                emit treeChanged();
            });
    connect(element, &FilterEllipse::valueChanged, this, [this](FilterEllipse* element, QSharedPointer<IFilterElement> selected, QVariant selectedOp,
                                                                QVariant old, QVariant value)
            {
                if(m_widgetToExp.contains(element))
                    if(auto exp = dynamic_cast<FilterExpression*>(m_widgetToExp[element]))
                    {
                        exp->SetFilter(selected->GetElementId(), selectedOp);
                        exp->SetFilterValue(value);
                    }
                
                emitFilterChanged();
                emit treeChanged();
            });
    
    element->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(element, &QWidget::customContextMenuRequested, this, [this, element](const QPoint& p)
            {
                auto menu = new QMenu();
                if(m_config.AllowSplittingToGroups)
                menu->addAction(tr("Split"), [this, element]()
                                {
                                    bool isFirstLevel = true;
                                    auto exp = m_widgetToExp[element];
                                    
                                    auto parentTest = exp->Parent();
                                    while(parentTest && parentTest != &m_root)
                                    {
                                        if(dynamic_cast<BracketsExpression*>(parentTest))
                                        {
                                            isFirstLevel = false;
                                            break;
                                        }
                                        parentTest = parentTest->Parent();
                                    }
                                    
                                    QSet<IBinaryOperator*> operators;
                                    bool toAndOr = false;
                                    if(!isFirstLevel)
                                    {
                                        parentTest->Tree([this, &operators](IExpression* exp)
                                                         {
                                                             if(auto binaryOp = dynamic_cast<IBinaryOperator*>(exp))
                                                             {
                                                                 operators.insert(binaryOp);
                                                             }
                                                         }, [](IExpression* exp) -> bool
                                                         {
                                                             if(dynamic_cast<BracketsExpression*>(exp))
                                                                 return false;
                                                             return true;
                                                         });
                                        
                                        if(operators.size() > 0)
                                        {
                                            if(dynamic_cast<AndOperator*>(operators.toList().first()))
                                                toAndOr = true;
                                        }
                                    }
                                    
                                    FilterEllipse* newElement = nullptr;
                                    if(isFirstLevel)
                                    {
                                        auto leftBr = new BracketLabel("(");
                                        auto rightBr = new BracketLabel(")");
                                        auto orWgt = CreateOperatorButton(toAndOr ? "&&" : "|");
                                        newElement = this->CreateFilterEllipse();
                                        
                                        auto parent = exp->Parent();
                                        auto brackets = new BracketsExpression(parent);
                                        brackets->SetData(new FilterRowNode({leftBr, rightBr}, [this](){return m_elementsContainer;}));
                                        auto newExp = new FilterExpression(nullptr);
                                        newExp->SetFilter(newElement->selectedFilter()->GetElementId(), newElement->selectedOperator());
                                        newExp->SetFilterValue(newElement->GetValue());
                                        newExp->SetData(new FilterRowNode({newElement}, [this](){return m_elementsContainer;}));
                                        auto orOp = toAndOr ?
                                            (IBinaryOperator*)new AndOperator(brackets, exp, newExp) :
                                            (IBinaryOperator*)new OrOperator(brackets, exp, newExp);
                                        orOp->SetData(new FilterRowNode({orWgt}, [this](){return m_elementsContainer;}));
                                        brackets->SetChild(orOp);
                                        newExp->SetParent(orOp);
                                        parent->SwapChild(exp, brackets);
                                        
                                        this->m_elementsContainer->insertWidgetBefore(leftBr, element);
                                        this->m_elementsContainer->insertWidgetAfter(orWgt, element);
                                        this->m_elementsContainer->insertWidgetAfter(newElement, orWgt);
                                        this->m_elementsContainer->insertWidgetAfter(rightBr, newElement);
                                        
                                        m_widgetToExp[newElement] = newExp;
                                        m_btnToExp[orWgt] = orOp;
                                    }
                                    else
                                    {
                                        auto orWgt = CreateOperatorButton(toAndOr ? "&&" : "|");
                                        newElement = this->CreateFilterEllipse();
                                        
                                        auto parent = exp->Parent();
                                        auto newExp = new FilterExpression(nullptr);
                                        newExp->SetFilter(newElement->selectedFilter()->GetElementId(), newElement->selectedOperator());
                                        newExp->SetFilterValue(newElement->GetValue());
                                        newExp->SetData(new FilterRowNode({newElement}, [this](){return m_elementsContainer;}));
                                        auto orOp = toAndOr ?
                                            (IBinaryOperator*)new AndOperator(parent, exp, newExp) :
                                            (IBinaryOperator*)new OrOperator(parent, exp, newExp);
                                        orOp->SetData(new FilterRowNode({orWgt}, [this](){return m_elementsContainer;}));
                                        newExp->SetParent(orOp);
                                        parent->SwapChild(exp, orOp);
                                        
                                        this->m_elementsContainer->insertWidgetAfter(orWgt, element);
                                        this->m_elementsContainer->insertWidgetAfter(newElement, orWgt);
                                        m_widgetToExp[newElement] = newExp;
                                        m_btnToExp[orWgt] = orOp;
                                    }
                                    
                                    newElement->selectFilter(element->selectedFilter());
                                    newElement->selectOperator(element->selectedOperator());
                                    
                                    m_elementsContainer->update();
                                    
                                    if(m_lineEdit && m_elementsContainer && isToLineEditAllowed())
                                        ui->frame->setCursor(Qt::IBeamCursor);
                                    else
                                        ui->frame->setCursor(Qt::ArrowCursor);
                                    
                                    emitFilterChanged();
                                    emit treeChanged();
                                });
                menu->addAction(tr("Remove"), [this, element]()
                                {
                                    this->removeFilterElement(element);
                                });
                menu->exec(element->mapToGlobal(p));
            });
    
    return element;
}

OperatorButton* FilterRow::CreateOperatorButton(QString text)
{
    auto btn = new OperatorButton(text, m_config.AllowOperatorChange);
    
    if(m_config.AllowOperatorChange)
    {
        connect(btn, &OperatorButton::myHoverChanged, this, [this, btn](bool hover)
                {
                    if(!m_btnToExp.contains(btn))
                        return;
                    auto exp = m_btnToExp[btn];
                    
                    while(exp && (exp != &m_root && dynamic_cast<BracketsExpression*>(exp) == nullptr))
                        exp = exp->Parent();
                    
                    if(!exp)
                        return;
                    
                    exp->Tree([this, hover](IExpression* exp)
                              {
                                  if(dynamic_cast<IBinaryOperator*>(exp))
                                  {
                                      OperatorButton* btn = nullptr;
                                      QList<QWidget*> wgts;
                                      if(exp->GetData<FilterRowNode>())
                                          wgts.append(exp->GetData<FilterRowNode>()->GetWidgets());
                                      if(wgts.size() > 0)
                                          btn = dynamic_cast<OperatorButton*>(wgts.first());
                                      if(btn)
                                          btn->setMyHoverSilent(hover);
                                  }
                              }, [](IExpression* exp) -> bool
                              {
                                  if(dynamic_cast<BracketsExpression*>(exp))
                                      return false;
                                  return true;
                              });
                });
        
        connect(btn, &OperatorButton::clicked, this, [this, btn]()
                {
                    if(!m_btnToExp.contains(btn))
                        return;
                    auto exp = m_btnToExp[btn];
                    
                    while(exp && (exp != &m_root && dynamic_cast<BracketsExpression*>(exp) == nullptr))
                        exp = exp->Parent();
                    
                    if(!exp)
                        return;
                    
                    QSet<IBinaryOperator*> operatorsToChange;
                    exp->Tree([this, &operatorsToChange](IExpression* exp)
                              {
                                  if(auto binaryOp = dynamic_cast<IBinaryOperator*>(exp))
                                  {
                                      operatorsToChange.insert(binaryOp);
                                  }
                              }, [](IExpression* exp) -> bool
                              {
                                  if(dynamic_cast<BracketsExpression*>(exp))
                                      return false;
                                  return true;
                              });
                    
                    if(operatorsToChange.size() > 0)
                    {
                        bool toOrAnd = false;
                        if(dynamic_cast<AndOperator*>(operatorsToChange.toList().first()))
                            toOrAnd = true;
                        
                        for(auto op : operatorsToChange)
                        {
                            auto data = op->GetData<FilterRowNode>();
                            op->SetData(nullptr);
                            QList<QWidget*> wgts;
                            if(data)
                                wgts.append(data->GetWidgets());
                            OperatorButton* opBtn = nullptr;
                            if(wgts.size() > 0)
                            {
                                auto wgt = wgts.first();
                                if(opBtn = dynamic_cast<OperatorButton*>(wgt))
                                    opBtn->SetText(toOrAnd ? "|" : "&&");
                            }
                            IBinaryOperator* newExp = toOrAnd ?
                                                          (IBinaryOperator*)new OrOperator(op->Parent(), op->Left(), op->Right()) :
                                                          (IBinaryOperator*)new AndOperator(op->Parent(), op->Left(), op->Right());
                            newExp->SetData(data);
                            
                            op->Left()->SetParent(newExp);
                            op->Right()->SetParent(newExp);
                            op->Parent()->SwapChild(op, newExp);
                            if(opBtn)
                                m_btnToExp[opBtn] = newExp;
                            op->SetLeft(nullptr); op->SetRight(nullptr);
                            delete op;
                        }
                    }
                    
                    emitFilterChanged();
    emit treeChanged();
                });
    }
    
    return btn;
}

void FilterRow::debugPrintTree()
{
#ifdef DEBUG_FILTER_TREE
    qDebug() << "------------------------";
    int level = 0;
    QVector<QString> result;
    m_root.Tree([&](IExpression* exp, QSet<QWidget*> wgts)
                {
                    QString tabs = "";
                    for(int i = 0; i < level; i++)
                        tabs += "  ";
                    if(dynamic_cast<IBinaryOperator*>(exp))
                    {
                        if(dynamic_cast<AndOperator*>(exp))
                            result += tabs + "AND";
                        else
                            result += tabs + "OR";
                    }
                    else if(dynamic_cast<FilterExpression*>(exp))
                    {
                        FilterEllipse* wgt = nullptr;
                        if(wgts.size() > 0)
                            wgt = dynamic_cast<FilterEllipse*>(wgts.toList().first());
                        QString r;
                        r += tabs + "Expression";
                        if(wgt)
                            r += " " + wgt->selectedFilter()->GetName() + "->" + wgt->GetValue().toString();
                        result += r;
                    }
                    else if(dynamic_cast<BracketsExpression*>(exp))
                    {
                        result += tabs + "()";
                    }
                }, [&](){level++;}, [&](){level--;}, true);
    for(int i = result.size() - 1; i >= 0; i--)
        qDebug() << result[i];
    qDebug() << "------------------------";
#endif
#ifdef DEBUG_FILTER_JSON
    qDebug() << "------------------------";
    qDebug().noquote().nospace() << QJsonDocument(m_root.ToJson()).toJson(QJsonDocument::Compact);
    qDebug() << "------------------------";
#endif
}

FilterEllipse* FilterRow::addFilterElement()
{
    if(this->GetOrderedFilters().size() == 0)
        return nullptr;
    
    auto element = CreateFilterEllipse();
    
    auto old = m_root.GetChild();
    FilterExpression* newFilterNode;
    if(old)
    {
        auto exp = &m_root;
        QSet<IBinaryOperator*> operators;
        exp->Tree([this, &operators](IExpression* exp)
                  {
                      if(auto binaryOp = dynamic_cast<IBinaryOperator*>(exp))
                      {
                          operators.insert(binaryOp);
                      }
                  }, [](IExpression* exp) -> bool
                  {
                      if(dynamic_cast<BracketsExpression*>(exp))
                          return false;
                      return true;
                  });
        
        bool toOrAnd = false;
        if(operators.size() > 0)
        {
            if(dynamic_cast<OrOperator*>(operators.toList().first()))
                toOrAnd = true;
        }
        
        auto label = CreateOperatorButton(toOrAnd ? "|" : "&&");
        
        auto newAndNode = toOrAnd ?
            (IBinaryOperator*)new OrOperator(&m_root, old, nullptr) :
            (IBinaryOperator*)new AndOperator(&m_root, old, nullptr);
        newAndNode->SetData(new FilterRowNode({label}, [this](){return m_elementsContainer;}));
        newFilterNode = new FilterExpression(newAndNode);
        newFilterNode->SetFilter(element->selectedFilter()->GetElementId(), element->selectedOperator());
        newFilterNode->SetFilterValue(element->GetValue());
        newFilterNode->SetData(new FilterRowNode({element}, [this](){return m_elementsContainer;}));
        newFilterNode->SetParent(newAndNode);
        newAndNode->SetRight(newFilterNode);
        m_root.SetChild(newAndNode);
        
        m_elementsContainer->insertWidgetBeforeLast(label);
        
        m_btnToExp[label] = newAndNode;
    }
    else
    {
        newFilterNode = new FilterExpression(&m_root);
        newFilterNode->SetFilter(element->selectedFilter()->GetElementId(), element->selectedOperator());
        newFilterNode->SetFilterValue(element->GetValue());
        newFilterNode->SetData(new FilterRowNode({element}, [this](){return m_elementsContainer;}));
        m_root.SetChild(newFilterNode);
    }
    m_elementsContainer->insertWidgetBeforeLast(element);
    
    m_widgetToExp[element] = newFilterNode;
    m_elementsContainer->update();
    
    if(m_lineEdit && m_elementsContainer && isToLineEditAllowed())
        ui->frame->setCursor(Qt::IBeamCursor);
    else
        ui->frame->setCursor(Qt::ArrowCursor);
    
    emitFilterChanged();
    emit treeChanged();
    return element;
}

void FilterRow::removeFilterElement(FilterEllipse* element)
{
    disconnect(element, &FilterEllipse::elementDeleted, this, &FilterRow::removeFilterElement);
    m_widgetToExp[element]->Delete();
    m_widgetToExp.remove(element);
    
    if(m_lineEdit && m_elementsContainer && isToLineEditAllowed())
        ui->frame->setCursor(Qt::IBeamCursor);
    else
        ui->frame->setCursor(Qt::ArrowCursor);
    
    emitFilterChanged();
    emit treeChanged();
}

void FilterRow::clearFilter()
{
    m_colorIndex = 0;
    delete m_root.GetChild();
    m_root.SetChild(nullptr);
    
    m_widgetToExp.clear();
    m_btnToExp.clear();
    
    if(m_lineEdit && m_elementsContainer && isToLineEditAllowed())
        ui->frame->setCursor(Qt::IBeamCursor);
    else
        ui->frame->setCursor(Qt::ArrowCursor);
    
    emitFilterChanged();
    emit treeChanged();
}

void FilterRow::search()
{
    if(m_config.IsTogglable)
    {
        if(m_toggled == false)
        {
            ui->enter->setStyleSheet("QPushButton"
                                     "{"
                                     "background-color: rgb(180, 130, 40);"
                                     "border: none;"
                                     "border-radius: 15px;"
                                     "}");
        }
        else
        {
            ui->enter->setStyleSheet("QPushButton"
                                     "{"
                                     "background-color: transparent;"
                                     "border: none;"
                                     "border-radius: 15px;"
                                     "}");
        }
        m_toggled = !m_toggled;
        emitFilterChanged();
    }
    else
    {
        emit searchClicked();
    }
}

void FilterRow::resizeEvent(QResizeEvent* event)
{
    if(m_elementsContainer)
        m_elementsContainer->update();
}

    
