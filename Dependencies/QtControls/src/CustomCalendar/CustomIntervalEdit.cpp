#include "CustomIntervalEdit.h"
#include "qgraphicseffect.h"
#include "ui_CustomIntervalEdit.h"
#include <QSpacerItem>
#include <QDebug>

CustomIntervalEdit::CustomIntervalEdit(QWidget* parent) :
    QFrame(parent), m_parent(parent),
    ui(new Ui::CustomIntervalEdit)
{
    ui->setupUi(this);
    
    connect(ui->inEdit, &CustomDateTimeEdit::movedToLastSection, this, [this]()
            {
                ui->outEdit->setCurrentSection(0);
                ui->outEdit->setFocus();
            });
    connect(ui->outEdit, &CustomDateTimeEdit::movedToTheLeft, this, [this]()
            {
                ui->inEdit->setCurrentSection(ui->inEdit->lastSection());
                ui->inEdit->setFocus();
            });
    connect(ui->inEdit, &CustomDateTimeEdit::editingFinished, this, [this](){
        emit editingFinished();
    });
    connect(ui->outEdit, &CustomDateTimeEdit::editingFinished, this, [this](){
        emit editingFinished();
    });
    connect(ui->inEdit, &CustomDateTimeEdit::editingCanceled, this, [this](){
        emit editingCanceled();
    });
    connect(ui->outEdit, &CustomDateTimeEdit::editingCanceled, this, [this](){
        emit editingCanceled();
    });
    
    {QSignalBlocker blocker{qApp};
        m_cal = new CustomCalendar(this->topLevelWidget(), ui->inEdit, ui->outEdit);
        
        auto calChildren = m_cal->findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
        m_cal->setFocusProxy(ui->inEdit);
        for(auto child : calChildren)
            child->setFocusProxy(ui->inEdit);
        
        m_cal->setWindowFlag(Qt::Widget, true);
        m_cal->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    }
    
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(5);
    effect->setXOffset(1);
    effect->setYOffset(1);
    effect->setColor(Qt::black);
    m_cal->setGraphicsEffect(effect);
    
    m_cal->SetSelectedSideStylesheet("font-weight: bold;");
    m_cal->setFocusPolicy(Qt::StrongFocus);
    m_cal->setAutoFillBackground(true);
    
    m_cal->SetIsRange(true);
    m_cal->SetHideOnOk(true);
    m_cal->hide();
    
    m_lastFocusedWidget = ui->inEdit;
    connect(qApp, &QApplication::focusChanged, this, &CustomIntervalEdit::onFocusChanged);
    
    QWidget* parentWgtIter = this;
    m_parentChangeWatcher = new _CustomIntervalEditParentChangeWatcher();
    while(parentWgtIter)
    {
        parentWgtIter->installEventFilter(m_parentChangeWatcher);
        parentWgtIter = parentWgtIter->parentWidget();
    }
    connect(m_parentChangeWatcher, &_CustomIntervalEditParentChangeWatcher::parentChanged, this, &CustomIntervalEdit::onTopLevelWidgetChanged);
    
    SetCalendarPopupShown(false);
    
    this->SetOut(QDateTime(QDate::currentDate(), QTime(23, 59, 59, 999)));
}

void CustomIntervalEdit::setHWidgetAlignment(Qt::Alignment alig, bool needRightIcon)
{
    if(curAlig == alig)
        return;

    if(rightSpacer)
    {
        ui->horizontalLayout_2->removeItem(rightSpacer);
        delete rightSpacer;
        rightSpacer = nullptr;
    }

    if(leftSpacer)
    {
        ui->horizontalLayout_2->removeItem(leftSpacer);
        delete leftSpacer;
        leftSpacer = nullptr;
    }

    needOutEditRightIcon = needRightIcon;

    if(needRightIcon)
        ui->outEdit->setMinimumWidth(defaultmaxOutIntervalWidth+28); // def size + icon size
    else
        ui->outEdit->setMinimumWidth(0);

    switch(alig)
    {
    case Qt::AlignLeft:
        curAlig.setFlag(Qt::AlignLeft);
        curAlig.setFlag(Qt::AlignRight, false);
        curAlig.setFlag(Qt::AlignHCenter, false);
        if(needRightIcon)
            setMaxWidthOutInterval(this->width());
        else
        {
            ui->horizontalLayout_2->insertSpacerItem(ui->horizontalLayout_2->count(), rightSpacer = new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
            ui->horizontalLayout_2->setStretch(ui->horizontalLayout_2->count() - 1,1);
        }
        break;
    case Qt::AlignRight:
        curAlig.setFlag(Qt::AlignRight);
        curAlig.setFlag(Qt::AlignLeft, false);
        curAlig.setFlag(Qt::AlignHCenter, false);
        ui->horizontalLayout_2->insertSpacerItem(0, leftSpacer = new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
        ui->horizontalLayout_2->setStretch(0,1);
        break;
    case Qt::AlignHCenter:
        curAlig.setFlag(Qt::AlignHCenter);
        curAlig.setFlag(Qt::AlignRight, false);
        curAlig.setFlag(Qt::AlignLeft, false);
        ui->horizontalLayout_2->insertSpacerItem(ui->horizontalLayout_2->count(), rightSpacer = new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
        ui->horizontalLayout_2->insertSpacerItem(0, leftSpacer = new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
        ui->horizontalLayout_2->setStretch(ui->horizontalLayout_2->count() - 1,1);
        ui->horizontalLayout_2->setStretch(0,1);
        break;
    default:
        break;
    }
}

void CustomIntervalEdit::setMaxWidthOutInterval(int width)
{
    maxOutIntervalWidth = width;
    ui->outEdit->setMaximumWidth(maxOutIntervalWidth);
}

void CustomIntervalEdit::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    if(needOutEditRightIcon && curAlig.testFlag(Qt::AlignLeft))
        setMaxWidthOutInterval(this->width());
    else
        setMaxWidthOutInterval(defaultmaxOutIntervalWidth);
}

void CustomIntervalEdit::onFocusChanged(QWidget* old, QWidget* now)
{
    if(m_showCal)
    {
        bool testFocus = TestFocus(now);
        if(m_cal->isHidden() && testFocus)
        {
            auto size = m_cal->size();
            auto position = QWidget::mapTo(m_cal->parentWidget(), this->rect().bottomLeft());
            auto center = QWidget::mapTo(m_cal->parentWidget(), this->rect().center());
            position.setX(center.x() - m_cal->size().width()/2);
            m_cal->setGeometry(position.x(),
                               position.y() + 9,
                               size.width(),
                               size.height());
            m_cal->show();
        }
        else if(testFocus == false)
        {
            m_cal->hide();
            emit lostFocus();
        }
    }
    
    if( now == ui->inEdit  ||
        now == ui->outEdit)
    {
        m_lastFocusedWidget = now;
        
        auto calChildren = m_cal->findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
        m_cal->setFocusProxy(now);
        for(auto child : calChildren)
            child->setFocusProxy(now);
    }
}

void CustomIntervalEdit::SetCalendarPopupShown(bool show)
{
    m_showCal = show;
    if(show == false)
        m_cal->hide();
}

CustomIntervalEdit::~CustomIntervalEdit()
{
    disconnect(qApp, &QApplication::focusChanged, this, &CustomIntervalEdit::onFocusChanged);
    delete m_cal;
    delete ui;
}

QDateTime CustomIntervalEdit::In()
{
    return ui->inEdit->dateTime();
}

QDateTime CustomIntervalEdit::Out()
{
    return ui->outEdit->dateTime();
}

void CustomIntervalEdit::SetIn(QDateTime in)
{
    ui->inEdit->setDateTime(in);
}

void CustomIntervalEdit::SetOut(QDateTime out)
{
    ui->outEdit->setDateTime(out);
}

CustomDateTimeEdit* CustomIntervalEdit::InEdit()
{
    return ui->inEdit;
}

CustomDateTimeEdit* CustomIntervalEdit::OutEdit()
{
    return ui->outEdit;
}

QLabel* CustomIntervalEdit::LabelDelimiter()
{
    return ui->label;
}

QFrame* CustomIntervalEdit::backgroundFrame()
{
    return ui->frame;
}

bool CustomIntervalEdit::TestFocus(QWidget* wgt)
{
    return isChildOf(wgt, this) || isChildOf(wgt, ui->inEdit) || isChildOf(wgt, ui->outEdit) || isChildOf(wgt, m_cal);
}

QWidget* CustomIntervalEdit::FocusProxy()
{
    return ui->inEdit->FocusProxy();
}

void CustomIntervalEdit::focusInEvent(QFocusEvent* event)
{
    auto cursorPos = this->mapFromGlobal(cursor().pos()).x();
    auto leftSide = ui->inEdit->geometry().right();
    auto rightSide = ui->outEdit->geometry().left();
    
    if(qAbs(cursorPos - leftSide) <= qAbs(cursorPos - rightSide))
        ui->inEdit->setFocus();
    else
        ui->outEdit->setFocus();
}

void CustomIntervalEdit::onTopLevelWidgetChanged()
{
    delete m_parentChangeWatcher;
    if(m_cal)
    {
        m_cal->setParent(this->topLevelWidget());
        
        m_cal->hide();
        qApp->processEvents();
        onFocusChanged(nullptr, qApp->focusWidget());
    }
    
    QWidget* parentWgtIter = this;
    m_parentChangeWatcher = new _CustomIntervalEditParentChangeWatcher();
    while(parentWgtIter)
    {
        parentWgtIter->installEventFilter(m_parentChangeWatcher);
        parentWgtIter = parentWgtIter->parentWidget();
    }
    
    connect(m_parentChangeWatcher, &_CustomIntervalEditParentChangeWatcher::parentChanged, this, &CustomIntervalEdit::onTopLevelWidgetChanged);    
}

