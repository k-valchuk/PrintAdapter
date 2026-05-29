#include "FilterPresetRow.h"
#include "ui_FilterPresetRow.h"
#include "FilterPresetListWidget.h"

#include <QPaintEvent>
#include <QHelpEvent>
#include <QResizeEvent>

FilterPresetRow::FilterPresetRow(FilterPresetListWidget* parent):
    DraggableFrame(parent),
    ui(new Ui::FilterPresetRow)
{
    m_parentList = parent;
    ui->setupUi(this);
    ui->lineEdit->setVisible(false);
    
    this->setCursor(Qt::PointingHandCursor);
    ui->label->setCursor(Qt::PointingHandCursor);
        
    this->setAttribute(Qt::WA_Hover, true);
    ui->dragIndicatorLabel->setStyleSheet("QLabel{background-color:transparent;}");
    
    ui->pushButton->setVisible(false);
    ui->pushButton_2->setVisible(false);
    
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
                emit editClicked();
                Edit();
            });
    connect(ui->pushButton_2, &QPushButton::clicked, this, [this]()
            {
                emit deleteClicked();
            });
    
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, [this]()
            {
                if (!ui->lineEdit->isModified())
                    return; // Ignore second signal.
                ui->lineEdit->setModified(false);
                
                emit nameEditRequested(ui->label->Text(), ui->lineEdit->text());
                SetEditing(false);
            });
}

FilterPresetRow::~FilterPresetRow()
{
    delete ui;
}

void FilterPresetRow::setOnHoverColor(QColor color)
{
    if(m_onHoverColor == color)
        return;
    m_onHoverColor = color;
    this->setStyleSheet(QString("QFrame[myHover=\"true\"]{background-color: rgba(%1, %2, %3, %4); border-radius: 2px;} "
                                "QToolTip{background-color: palette(window); border-radius: 2px; padding: 0px; margin: 0px; color: palette(text);}")
                            .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
}

void FilterPresetRow::setOnHoverTextColor(QColor color)
{
    if(m_onHoverTextColor == color)
        return;
    m_onHoverTextColor = color;
    if(m_myHover)
        for(auto label : QList<QLabel*>{ui->label, ui->numLabel})
            label->setStyleSheet(QString("QLabel{background-color:transparent; color: rgba(%1, %2, %3, %4);}")
                                     .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
    else
        for(auto label : QList<QLabel*>{ui->label, ui->numLabel})
            label->setStyleSheet("QLabel{background-color:transparent;}");
}

void FilterPresetRow::setMyHoverPrivate(bool isHover)
{
    if(m_parentList->m_currentDraggable)
        isHover = false;
    
    m_myHover = isHover;
    if(isHover)
    {
        for(auto label : QList<QLabel*>{ui->label, ui->numLabel})
            label->setStyleSheet(QString("QLabel{background-color:transparent; color: rgba(%1, %2, %3, %4);}")
                .arg(m_onHoverTextColor.red()).arg(m_onHoverTextColor.green()).arg(m_onHoverTextColor.blue()).arg(m_onHoverTextColor.alpha()));
        ui->pushButton->setVisible(true);
        ui->pushButton_2->setVisible(true);
    }
    else
    {
        for(auto label : QList<QLabel*>{ui->label, ui->numLabel})
            label->setStyleSheet("QLabel{background-color:transparent;}");
        ui->pushButton->setVisible(false);
        ui->pushButton_2->setVisible(false);
    }
    style()->polish(this);
}

bool FilterPresetRow::event(QEvent* event)
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

void FilterPresetRow::SetEditing(bool isEdit)
{
    ui->horizontalSpacer->changeSize(1, 1, isEdit ? QSizePolicy::Fixed : QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    ui->numLabel->setVisible(!isEdit);
    if(isEdit)
    {
        ui->lineEdit->setText(ui->label->Text());
        ui->lineEdit->setModified(true);
    }
    ui->label->setVisible(!isEdit);
    ui->lineEdit->setVisible(isEdit);
    if(isEdit)
        ui->lineEdit->setFocus();
}

void FilterPresetRow::Edit()
{
    SetEditing(true);
}

void FilterPresetRow::SetName(QString name)
{
    ui->label->setTextAndElide(name);
}

void FilterPresetRow::SetPrefix(QString prefix)
{
    ui->numLabel->setText(prefix);
}
