#include "CustomCalendar.h"
#include "qstyle.h"
#include "qdebug.h"
#include "CustomDateTimeEdit.h"
#include "ui_CustomCalendar.h"

QString CustomCalendar::GetMonthName(int month)
{
    switch(month)
    {
    case 1:
        return tr("January");
    case 2:
        return tr("February");
    case 3:
        return tr("March");
    case 4:
        return tr("April");
    case 5:
        return tr("May");
    case 6:
        return tr("June");
    case 7:
        return tr("July");
    case 8:
        return tr("August");
    case 9:
        return tr("September");
    case 10:
        return tr("October");
    case 11:
        return tr("November");
    case 12:
        return tr("December");
    
    default:
        return "";
    }
}

QString CustomCalendar::GetShortMonthName(int month)
{
    switch(month)
    {
    case 1:
        return tr("jan");
    case 2:
        return tr("feb");
    case 3:
        return tr("march");
    case 4:
        return tr("apr");
    case 5:
        return tr("may");
    case 6:
        return tr("june");
    case 7:
        return tr("july");
    case 8:
        return tr("aug");
    case 9:
        return tr("sep");
    case 10:
        return tr("oct");
    case 11:
        return tr("nov");
    case 12:
        return tr("dec");
    
    default:
        return "";
    }
}

void CustomCalendar::UpdateButtonsStyles()
{
    if(m_currentMode == Mode::Months){
        auto year = ui->calendarWidget->yearShown();
        for(int b = 0; b < 12; b++) {
            auto month = QDate(year, m_btnsData[b], 1);
            
            if(month == QDate(ui->inEdit->date().year(), ui->inEdit->date().month(), 1) ||
                (m_isRange && month == QDate(ui->outEdit->date().year(), ui->outEdit->date().month(), 1)))
                m_btns[b]->setStyleSheet("background-color: rgb(69, 106, 158); border: none; color: white;");
            else if(m_isRange &&
                     month > QDate(ui->inEdit->date().year(), ui->inEdit->date().month(), 1) && month < QDate(ui->outEdit->date().year(), ui->outEdit->date().month(), 1))
                m_btns[b]->setStyleSheet("background-color: rgba(69, 106, 158, 77); border: none; color: white;");
            else
                m_btns[b]->setStyleSheet("");
        }
    }
    else if(m_currentMode == Mode::Years) {
        for(int b = 0; b < 12; b++) {
            auto year = m_btnsData[b];
            
            if(year == ui->inEdit->date().year() ||
                (m_isRange && year == ui->outEdit->date().year()))
                m_btns[b]->setStyleSheet("background-color: rgb(69, 106, 158); border: none; color: white;");
            else if(m_isRange && year > ui->inEdit->date().year() && year < ui->outEdit->date().year())
                m_btns[b]->setStyleSheet("background-color: rgba(69, 106, 158, 77); border: none; color: white;");
            else
                m_btns[b]->setStyleSheet("");
        }
    }
}

void CustomCalendar::SetMode(Mode mode, bool setSection)
{
    m_currentMode = mode;
    if(mode == Mode::Days) {
        SetCurrentIndex(true);
        
        if(setSection)
        {
            if(m_editingIn)
                ui->inEdit->setCurrentSection(0);
            else
                ui->outEdit->setCurrentSection(0);
        }
    }
    else {
        if(mode == Mode::Months){
            for(int month = 1; month <= 12; month++) {
                m_btns[month - 1]->setText(GetShortMonthName(month));
                m_btnsData[month - 1] = month;
            }
            
            if(setSection)
            {
                if(m_editingIn)
                    ui->inEdit->setCurrentSection(3);
                else
                    ui->outEdit->setCurrentSection(3);
            }
        }
        else if(mode == Mode::Years) {
            auto yearShown = ui->calendarWidget->yearShown();
            int startYear = yearShown - 6;
            
            if(m_isRange) { //ОШИБКА ЭТО ЛОМАЕТ ЛИСТАНИЕ ПО 12 ЛЕТ
                auto inYear = ui->inEdit->date().year();
                auto outYear = ui->outEdit->date().year();
                
                if(qAbs(outYear - inYear) > 11)
                    ;
                else
                    startYear = (qMin(inYear, outYear) + qRound(qAbs(outYear - inYear)/2.0)) - 6;
                
                while(!(yearShown >= startYear && yearShown <= startYear + 11))
                    if(yearShown < startYear)
                        startYear-=12;
                    else
                        startYear+=12;
            }
            
            for(int year = startYear, b = 0; b < 12; year++, b++) {
                m_btns[b]->setText(QString("%1").arg(year));
                m_btnsData[b] = year;
            }
            
            if(setSection)
            {
                if(m_editingIn)
                    ui->inEdit->setCurrentSection(6);
                else
                    ui->outEdit->setCurrentSection(6);
            }
        }
        
        SetCurrentIndex(false);
    }
    
    UpdateButtonsStyles();
}

void CustomCalendar::SetEditingSide(bool editingIn)
{
    if(m_isRange == false)
        return;
    
    m_editingIn = editingIn;
    
    auto highlightedStyle = m_selectedBorderStyle;
    
    ui->inEdit->setStyleSheet(m_editingIn ? m_baseEditStyle + highlightedStyle : m_baseEditStyle + "");
    ui->outEdit->setStyleSheet(m_editingIn ? m_baseEditStyle + "" : m_baseEditStyle + highlightedStyle);
    
//КУТЭ 5.12 не хочет обновлять стиль у детей в соответствии с родительским
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    auto children = ui->inEdit->findChildren<QWidget*>(QString());
    children += ui->outEdit->findChildren<QWidget*>(QString());
    for(auto wgt : children)
        wgt->style()->polish(wgt);
#endif
    
    auto edit = m_editingIn ? ui->inEdit : ui->outEdit;
    auto page = QPair<int, int>(ui->calendarWidget->yearShown(), ui->calendarWidget->monthShown());
    ui->calendarWidget->setSelectedDate(edit->date());
    if(m_scrollCalendarOnEditorsFocus == false)
        ui->calendarWidget->setCurrentPage(page.first, page.second);
    
    UpdateButtonsStyles();
}

void CustomCalendar::SetCurrentIndex(bool onCalendarPage)
{
    if(m_onCalendarPage == onCalendarPage)
        return;
    
    if(onCalendarPage == true)
    {
        ui->buttonsFrame->hide();
        ui->calendarWidget->show();
    }
    else
    {
        ui->calendarWidget->hide();
        ui->buttonsFrame->show();
    }
    
    m_onCalendarPage = onCalendarPage;
}

bool CustomCalendar::coerceDt(QDateTime& dt, const std::optional<QDateTime>& min, const std::optional<QDateTime>& max)
{
    bool modified = false;
    if(min && dt < *min)
    {
        dt = *min;
        modified = true;
    }
    if(max && dt > *max)
    {
        dt = *max;
        modified = true;
    }
    return modified;
}

CustomCalendar::CustomCalendar(QWidget *parent, CustomDateTimeEdit* otherIn, CustomDateTimeEdit* otherOut) :
    QFrame(parent),
    ui(new Ui::CustomCalendar)
{
    ui->setupUi(this);
    ui->buttonsFrame->hide();
    
    {
        m_btns[0] = ui->btn1;
        m_btns[1] = ui->btn2;
        m_btns[2] = ui->btn3;
        m_btns[3] = ui->btn4;
        m_btns[4] = ui->btn5;
        m_btns[5] = ui->btn6;
        m_btns[6] = ui->btn7;
        m_btns[7] = ui->btn8;
        m_btns[8] = ui->btn9;
        m_btns[9] = ui->btn10;
        m_btns[10] = ui->btn11;
        m_btns[11] = ui->btn12;
    }
    
    for(int b = 0; b < 12; b++)
    {
        connect(m_btns[b], &QPushButton::clicked, this, [this, b]()
                {
                    if(m_currentMode == Mode::Days)
                        return;
                    
                    auto year = ui->calendarWidget->yearShown();
                    auto month = ui->calendarWidget->monthShown();
                    
                    if(m_currentMode == Mode::Months) {
                        ui->calendarWidget->setCurrentPage(year, m_btnsData[b]);
                        SetMode(Mode::Days);
                    }
                    else if(m_currentMode == Mode::Years) {
                        ui->calendarWidget->setCurrentPage(m_btnsData[b], month);
                        SetMode(Mode::Months);
                    }
                });
    }
    
    ui->calendarWidget->setSelectedDate(QDate::currentDate());
    if(otherIn)
    {
        ui->InLabel->hide();
        ui->inEdit->hide();
        
        ui->inEdit = otherIn;
        m_hideIn = true;
        
        ui->calendarWidget->setSelectedDate(ui->inEdit->date());
    }
    else
    {
        ui->inEdit->setDate(QDate::currentDate());
        ui->inEdit->setTime(QTime::fromMSecsSinceStartOfDay(0));
    }
    if(otherOut)
    {
        ui->OutLabel->hide();
        ui->outEdit->hide();
        
        ui->outEdit = otherOut;
        m_hideOut = true;
    }
    else
    {
        ui->outEdit->setDate(QDate::currentDate());
        ui->outEdit->setTime(QTime::fromMSecsSinceStartOfDay(0));
    }
    
    m_baseEditStyle = ui->inEdit->styleSheet();
    
    SetIsRange(m_isRange);
    
    ui->calendarWidget->setNavigationBarVisible(false);
    ui->calendarWidget->SetParentCustomCalendar(this);
    
    ui->btnMonth->setText(GetMonthName(QDate::currentDate().month()));
    ui->btnYear->setText(QString("%1").arg(QDate::currentDate().year()));
    
    SetMode(Mode::Days, false);
    
    connect(ui->calendarWidget, &QCalendarWidget::currentPageChanged, this, [this](int year, int month)
            {
                ui->btnMonth->setText(GetMonthName(month));
                ui->btnYear->setText(QString("%1").arg(year));
            });
    
    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, [this]()
            {
                //QSignalBlocker inBlocker{ui->inEdit};
                //QSignalBlocker outBlocker{ui->outEdit};
                
                auto date = ui->calendarWidget->selectedDate();
                if(m_isRange) {
                    if(m_editingIn){
                        auto newInDt = QDateTime(date, ui->inEdit->time());
                        if(coerceDt(newInDt, m_minDtIn, m_maxDtIn))
                        {
                            QSignalBlocker bl{ui->calendarWidget};
                            ui->calendarWidget->setSelectedDate(newInDt.date());
                            date = newInDt.date();
                        }
                        if(newInDt <= ui->outEdit->dateTime())
                            ui->inEdit->setDate(date);
                        else
                        {
                            SetEditingSide(false);
                            ui->outEdit->setFocus();
                            ui->outEdit->setDate(date);
                        }
                    }
                    else {
                        auto newOutDt = QDateTime(date, ui->outEdit->time());
                        if(coerceDt(newOutDt, m_minDtOut, m_maxDtOut))
                        {
                            QSignalBlocker bl{ui->calendarWidget};
                            ui->calendarWidget->setSelectedDate(newOutDt.date());
                            date = newOutDt.date();
                        }
                        if(newOutDt >= ui->inEdit->dateTime())
                            ui->outEdit->setDate(date);
                        else
                        {
                            SetEditingSide(false);
                            ui->inEdit->setFocus();
                            ui->inEdit->setDate(date);
                        }
                    }
                }
                else
                    ui->inEdit->setDate(date);
                
                //emit datesChanged(InDateTime(), OutDateTime());
            });
    
    connect(ui->inEdit, &CustomDateTimeEdit::dateTimeChanged, this, [this](QDateTime dateTime)
            {
                auto in = ui->inEdit->dateTime();
                if(coerceDt(in, m_minDtIn, m_maxDtIn))
                {QSignalBlocker bl{ui->inEdit};
                    ui->inEdit->setDateTime(in);
                }
                auto out = ui->outEdit->dateTime();
                
                ui->calendarWidget->setCurrentPage(in.date().year(), in.date().month());
                QSignalBlocker blocker{ui->calendarWidget};
                ui->calendarWidget->setSelectedDate(in.date());
                if(m_currentMode != Mode::Days) SetMode(m_currentMode);
                
                if(in == out && m_isRange)
                {
                    ui->InLabel->setStyleSheet("color: red");
                    ui->InLabel->setToolTip(tr("Warning! In and out time points are equal!"));
                    ui->OutLabel->setStyleSheet("");
                    ui->OutLabel->setToolTip("");
                }
                else if(in > out && m_isRange)
                {
                    QSignalBlocker b {ui->outEdit};
                    ui->outEdit->setDateTime(in);
                    
                    ui->OutLabel->setStyleSheet("color: red");
                    ui->OutLabel->setToolTip(tr("Warning! In and out time points are equal!"));
                    ui->InLabel->setStyleSheet("");
                    ui->InLabel->setToolTip("");
                }
                else
                {
                    ui->InLabel->setStyleSheet("");
                    ui->InLabel->setToolTip("");
                    ui->OutLabel->setStyleSheet("");
                    ui->OutLabel->setToolTip("");
                }
                
                emit datesChanged(InDateTime(), OutDateTime());
            });
    connect(ui->outEdit, &CustomDateTimeEdit::dateTimeChanged, this, [this](QDateTime dateTime)
            {
                if(m_isRange == false)
                    return;
                
                auto in = ui->inEdit->dateTime();
                auto out = ui->outEdit->dateTime();
                if(coerceDt(out, m_minDtOut, m_maxDtOut))
                {QSignalBlocker bl{ui->outEdit};
                    ui->outEdit->setDateTime(out);
                }
                
                ui->calendarWidget->setCurrentPage(out.date().year(), out.date().month());
                QSignalBlocker blocker{ui->calendarWidget};
                ui->calendarWidget->setSelectedDate(out.date());
                if(m_currentMode != Mode::Days) SetMode(m_currentMode);
                
                if(out == in && m_isRange)
                {
                    ui->OutLabel->setStyleSheet("color: red");
                    ui->OutLabel->setToolTip(tr("Warning! In and out time points are equal!"));
                    ui->InLabel->setStyleSheet("");
                    ui->InLabel->setToolTip("");
                }
                else if(out < in && m_isRange)
                {
                    QSignalBlocker b1{ui->inEdit};
                    ui->inEdit->setDateTime(out);
                    
                    ui->InLabel->setStyleSheet("color: red");
                    ui->InLabel->setToolTip(tr("Warning! In and out time points are equal!"));
                    ui->OutLabel->setStyleSheet("");
                    ui->OutLabel->setToolTip("");
                }
                else
                {
                    ui->InLabel->setStyleSheet("");
                    ui->InLabel->setToolTip("");
                    ui->OutLabel->setStyleSheet("");
                    ui->OutLabel->setToolTip("");
                }
                
                emit datesChanged(InDateTime(), OutDateTime());
            });
    
    connect(ui->todayBtn, &QPushButton::clicked, this, [this]()
            {
                SetMode(Mode::Days);
                auto curDate = QDate::currentDate();
                ui->calendarWidget->setCurrentPage(curDate.year(), curDate.month());
                if(m_isRange == false)
                    ui->calendarWidget->setSelectedDate(curDate);
            });
    
    auto smallFwd = [this]()
    {
        if(m_currentMode == Mode::Days) {
            ui->calendarWidget->showNextMonth();
        }
        else if(m_currentMode == Mode::Months) {
            ui->calendarWidget->showNextYear();
            SetMode(m_currentMode); //update buttons text
        }
        else if(m_currentMode == Mode::Years) {
            auto year  = ui->calendarWidget->yearShown();
            auto month = ui->calendarWidget->monthShown();
            year+=12;
            ui->calendarWidget->setCurrentPage(year, month);
            SetMode(m_currentMode); //update buttons text
        }
    };
    auto smallBack = [this]()
    {
        if(m_currentMode == Mode::Days) {
            ui->calendarWidget->showPreviousMonth();
        }
        else if(m_currentMode == Mode::Months) {
            ui->calendarWidget->showPreviousYear();
            SetMode(m_currentMode); //update buttons text
        }
        else if(m_currentMode == Mode::Years) {
            auto year  = ui->calendarWidget->yearShown();
            auto month = ui->calendarWidget->monthShown();
            year-=12;
            ui->calendarWidget->setCurrentPage(year, month);
            SetMode(m_currentMode); //update buttons text
        }
    };
    connect(ui->smallFwdBtn, &QPushButton::clicked, this, smallFwd);
    connect(ui->smallBackBtn, &QPushButton::clicked, this, smallBack);
    connect(ui->buttonsFrame, &ScrollableCalendarFrame::scrolledDown, smallFwd);
    connect(ui->buttonsFrame, &ScrollableCalendarFrame::scrolledUp, smallBack);
    
    connect(ui->okBtn, &QPushButton::clicked, this, [this]()
            {
                emit okPressed(InDateTime(), OutDateTime());
                if(m_hideOnOk)
                    this->hide();
            });
    
    connect(ui->btnMonth, &QPushButton::clicked, this, [this]()
            {
                SetMode(Mode::Months);
            });
    connect(ui->btnYear, &QPushButton::clicked, this, [this]()
            {
                SetMode(Mode::Years);
            });
    
    
    connect(qApp, &QApplication::focusChanged, this, [this](QWidget* old, QWidget* now)
            {
                if(m_isRange == false)
                    return;
                
                auto isChildOf = [](QWidget* child, QWidget* parent) -> bool {
                    while(child) {
                        if(child == parent)
                            return true;
                        child = child->parentWidget();
                    }
                    return false;
                };
                
                if(isChildOf(now, ui->inEdit)) {
                    SetEditingSide(true);
                }
                else if(isChildOf(now, ui->outEdit)) {
                    SetEditingSide(false);
                }
            });
}

CustomCalendar::~CustomCalendar()
{
    delete ui;
}

QDateTime CustomCalendar::InDateTime()
{
    return ui->inEdit->dateTime();
}

QDateTime CustomCalendar::OutDateTime()
{
    return ui->outEdit->dateTime();
}

CustomDateTimeEdit* CustomCalendar::InDateTimeEdit()
{
    return ui->inEdit;
}

void CustomCalendar::SetInDateTime(const QDateTime& dt)
{
    ui->inEdit->setDateTime(dt);
}

void CustomCalendar::SetOutDateTime(const QDateTime& dt)
{
    ui->outEdit->setDateTime(dt);
}

void CustomCalendar::SetInMinMaxDateTimes(const std::optional<QDateTime>& min, const std::optional<QDateTime>& max)
{
    m_minDtIn = min;
    m_maxDtIn = max;
    
    auto inDt = ui->inEdit->dateTime();
    if(coerceDt(inDt, min, max))
        SetInDateTime(inDt);
}

void CustomCalendar::SetOutMinMaxDateTimes(const std::optional<QDateTime>& min, const std::optional<QDateTime>& max)
{
    m_minDtOut = min;
    m_maxDtOut = max;
    
    auto outDt = ui->outEdit->dateTime();
    if(coerceDt(outDt, min, max))
        SetOutDateTime(outDt);
}

void CustomCalendar::SetHideOnOk(bool hideOnOk)
{
    m_hideOnOk = hideOnOk;
    ui->okBtn->setVisible(!m_hideOnOk);
}

void CustomCalendar::SetIsRange(bool isRange)
{
    m_isRange = isRange;
    if(m_isRange)
    {
        SetEditingSide(true);
        ui->outEdit->setVisible(true);
        ui->OutLabel->setVisible(true && !m_hideOut);
        ui->InLabel->setVisible(true && !m_hideIn);
        
        ui->gridLayout_2->removeWidget(ui->okBtn);
        ui->gridLayout_2->addWidget(ui->okBtn, 1, 2);
    }
    else
    {
        m_editingIn = true;
        ui->outEdit->setVisible(false);
        ui->OutLabel->setVisible(false && !m_hideOut);
        ui->InLabel->setVisible(false && !m_hideIn);
        
        ui->gridLayout_2->removeWidget(ui->okBtn);
        ui->gridLayout_2->addWidget(ui->okBtn, 0, 2);
    }
}

void CustomCalendar::SetSelectedSideStylesheet(const QString& style)
{
    m_selectedBorderStyle = style;
    SetEditingSide(m_editingIn);
}

void CustomCalendar::showEvent(QShowEvent* event)
{
    SetMode(Mode::Days, false);
}
