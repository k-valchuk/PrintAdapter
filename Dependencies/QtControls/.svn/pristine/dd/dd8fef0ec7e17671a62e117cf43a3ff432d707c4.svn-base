#include "FilterHistoryRow.h"
#include "qbuffer.h"
#include "qpainter.h"
#include "ui_FilterHistoryRow.h"

#include "FilterHistoryListWidget.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QToolTip>
#include "FilterRow.h"

FilterHistoryRow::FilterHistoryRow(FilterHistoryListWidget* parent):
    DraggableFrame(parent),
    ui(new Ui::FilterHistoryRow)
{
    m_parentList = parent;
    ui->setupUi(this);
    
    this->setCursor(Qt::PointingHandCursor);
    ui->label->setCursor(Qt::PointingHandCursor);
    
    this->setAttribute(Qt::WA_Hover, true);
    ui->dragIndicatorLabel->setStyleSheet("QLabel{background-color:transparent;}");
    
    ui->pushButton->setVisible(false);
    
    connect(this, &DraggableFrame::dragAboutToStart, [this]()
            {
                ui->dragIndicatorLabel->setText("::");
                this->setMyHover(true);
            });
    connect(this, &DraggableFrame::dragCanceled, [this]()
            {
                ui->dragIndicatorLabel->setText("");
            });
    connect(this, &DraggableFrame::dropped, [this]()
            {
                ui->dragIndicatorLabel->setText("");
            });
    
    connect(ui->pushButton, &QPushButton::clicked, this, [this]()
            {
                emit saveClicked();
            });
}

FilterHistoryRow::~FilterHistoryRow()
{
    delete ui;
}

void FilterHistoryRow::setOnHoverColor(QColor color)
{
    if(m_onHoverColor == color)
        return;
    m_onHoverColor = color;
    this->setStyleSheet(QString("QFrame[myHover=\"true\"]{background-color: rgba(%1, %2, %3, %4); border-radius: 2px;} "
                                "QToolTip{background-color: palette(window); border-radius: 2px; padding: 0px; margin: 0px; color: palette(text);}")
                            .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
}

void FilterHistoryRow::setOnHoverTextColor(QColor color)
{
    if(m_onHoverTextColor == color)
        return;
    m_onHoverTextColor = color;
    if(m_myHover)
        for(auto label : QList<QLabel*>{ui->label})
            label->setStyleSheet(QString("QLabel{background-color:transparent; color: rgba(%1, %2, %3, %4);}")
                                     .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
    else
        for(auto label : QList<QLabel*>{ui->label})
            label->setStyleSheet("QLabel{background-color:transparent;}");
}

void FilterHistoryRow::setMyHoverPrivate(bool isHover)
{
    if(m_parentList->m_currentDraggable)
        isHover = false;
    
    m_myHover = isHover;
    if(isHover)
    {
        for(auto label : QList<QLabel*>{ui->label})
            label->setStyleSheet(QString("QLabel{background-color:transparent; color: rgba(%1, %2, %3, %4);}")
                                     .arg(m_onHoverTextColor.red()).arg(m_onHoverTextColor.green()).arg(m_onHoverTextColor.blue()).arg(m_onHoverTextColor.alpha()));
        ui->pushButton->setVisible(true);
    }
    else
    {
        for(auto label : QList<QLabel*>{ui->label})
            label->setStyleSheet("QLabel{background-color:transparent;}");
        ui->pushButton->setVisible(false);
    }
    style()->polish(this);
}

bool FilterHistoryRow::event(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::HoverEnter:
        setProperty("myHover", true);
        //return true;
        break;
    case QEvent::HoverLeave:
        setProperty("myHover", false);
        //return true;
        break;
    case QEvent::HoverMove:
        //return true;
        break;
    case QEvent::ToolTip:
        ShowTooltip(((QHelpEvent*)event)->globalPos(), this);
        break;
    default:
        break;
    }
    
    return QFrame::event(event);
}

void FilterHistoryRow::SetName(QString name)
{
    ui->label->setTextAndElide(name);
}
