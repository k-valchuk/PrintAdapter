#include "FilterPanel.h"
#include "ui_FilterPanel.h"
#include <QMessageBox>

FilterPanel::FilterPanel(const FilterRow::FilterRowConfig& filterRowConfig, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FilterPanel)
{
    ui->setupUi(this);
    ui->quickFilter->SetOtherQuickFilterTabWgt(ui->quickFilterTabWgt);
    
    m_filterRow = new FilterRow(filterRowConfig, this);
    ui->filterRowLayout->insertWidget(3, m_filterRow);
    
    ui->splitter->setStretchFactor(0, 3);
    ui->splitter->setStretchFactor(1, 7);
    ui->splitter->setCollapsible(1, false);
    
    
    connect(m_filterRow, &FilterRow::filterChanged, this, [this](QSharedPointer<IExpression> rowExp)
            {
                m_filterRowExp = rowExp;
                
                updateWholeFilterExp();
                emit filterChanged();
                emit filterChangedData(QSharedPointer<IExpression>(m_filterExp.isNull()? nullptr : m_filterExp->Copy()),
                                       QSharedPointer<IExpression>(m_quickFilterExp.isNull()? nullptr : m_quickFilterExp->Copy()),
                                       QSharedPointer<IExpression>(m_filterRowExp.isNull()? nullptr : m_filterRowExp->Copy()));
            });
    connect(ui->quickFilter, &QuickFilterPanel::QuickFilterChanged, this, [this](QSharedPointer<IExpression> qfExp)
            {
                m_quickFilterExp = qfExp;
                
                updateWholeFilterExp();
                emit filterChanged();
                emit filterChangedData(QSharedPointer<IExpression>(m_filterExp.isNull()? nullptr : m_filterExp->Copy()),
                                       QSharedPointer<IExpression>(m_quickFilterExp.isNull()? nullptr : m_quickFilterExp->Copy()),
                                       QSharedPointer<IExpression>(m_filterRowExp.isNull()? nullptr : m_filterRowExp->Copy()));
            });
    
    connect(this->QuickFilterWidget(), &QuickFilterPanel::FilterPresetApplied, this, [this](QSharedPointer<IExpression> exp)
            {
                bool f = true;
                if(!this->FilterRowWidget()->IsEmpty())
                {
                    f = false;
                    QMessageBox::StandardButton reply;
                    reply = QMessageBox::question(nullptr, "Apply filter",
                                                  "Filter row is not empty, "
                                                  "do you wish to apply another filter preset?",
                                                  QMessageBox::Yes|QMessageBox::No);
                    
                    if (reply == QMessageBox::Yes)
                        f = true;
                }
                
                if(f)
                    this->FilterRowWidget()->restoreFromTree(exp ? exp->Copy() : nullptr);
            });
}

void FilterPanel::updateWholeFilterExp()
{
    if(m_filterRowExp && m_quickFilterExp)
    {
        m_filterExp = QSharedPointer<IExpression>(
            Concat<AndOperator>(m_filterRowExp->Copy(), m_quickFilterExp->Copy())
            );
    }
    else if(m_filterRowExp)
        m_filterExp = QSharedPointer<IExpression>(m_filterRowExp->Copy());
    else if(m_quickFilterExp)
        m_filterExp = QSharedPointer<IExpression>(m_quickFilterExp->Copy());
    else
        m_filterExp = nullptr;
}

FilterPanel::~FilterPanel()
{
    delete ui;
}

QLayout* FilterPanel::ContentLayout()
{
    return ui->contentContainerVLayout;
}

QLayout* FilterPanel::NavBarContentLayout()
{
    return ui->navBarContainerHLayout;
}

void FilterPanel::SetFilterRowVisible(bool visible)
{
    ui->filterRowFrame->setVisible(visible);
    ui->line->setVisible(visible);
    
    emit filterRowVisibleChanged(visible);
}

bool FilterPanel::IsFilterRowVisible()
{
    return ui->filterRowFrame->isHidden() == false;
}

QuickFilterPanel* FilterPanel::QuickFilterWidget()
{
    return ui->quickFilter;
}


