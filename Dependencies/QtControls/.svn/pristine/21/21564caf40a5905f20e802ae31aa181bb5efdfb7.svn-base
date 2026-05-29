#include "CustomDateTimeEdit.h"
#include "qboxlayout.h"
#include "qgraphicseffect.h"
#include <QValidator>
#include <QDateTime>
#include "CustomCalendar.h"
#include "qdebug.h"

class DateTimeValidator : public QValidator
{
    mutable QString lastValidDay = "";
    mutable QString lastValidMonth = "";
    mutable QString lastValidYear = "";
    
    mutable QString lastValidHour = "";
    mutable QString lastValidMinute = "";
    mutable QString lastValidSecond = "";
    mutable QString lastValidMsOrFrames = "";
    
    CustomDateTimeEdit::Format m_format;
public:
    DateTimeValidator(const CustomDateTimeEdit::Format& format)
        : m_format(format)
    {
        auto& formatStr = m_format.format;
        //опустить буквы часов, секунд, миллисекунд
        //потому что в Qt-формате они в принципе поддерживаются
        //но в данной валидации разницы нет
        for(int i = 0; i < formatStr.size(); i++)
        {
            if(formatStr[i] == 'H')
                formatStr[i] = 'h';
            else if(formatStr[i] == 'S')
                formatStr[i] = 's';
            else if(formatStr[i] == 'Z')
                formatStr[i] = 'z';
            else if(formatStr[i] == 'F')
                formatStr[i] = 'f';
        }
    }
    
    State validate(QString& strDt, int& pos) const override
    {
        auto& str = strDt;
        bool dateAcceptable = !(m_format.format.contains('d') || m_format.format.contains('M') || m_format.format.contains('y'));
        
        int d1 = -1, d2 = -1, M1 = -1, M2 = -1, y1 = -1, y2 = -1, y3 = -1, y4 = -1;
        d1 = m_format.format.indexOf('d');
        if(d1 >= 0)
            d2 = m_format.format.indexOf('d', d1 + 1);
        M1 = m_format.format.indexOf('M');
        if(M1 >= 0)
            M2 = m_format.format.indexOf('M', M1 + 1);
        y1 = m_format.format.indexOf('y');
        if(y1 >= 0)
            y2 = m_format.format.indexOf('y', y1 + 1);
        if(y2 >= 0)
            y3 = m_format.format.indexOf('y', y2 + 1);
        if(y3 >= 0)
            y4 = m_format.format.indexOf('y', y3 + 1);
        
        QString day, month, year;
        if(d1 >= 0 && d2 >= 0)
            day = QString(strDt[d1]) + QString(strDt[d2]);
        if(M1 >= 0 && M2 >= 0)
            month = QString(strDt[M1]) + QString(strDt[M2]);
        if(y1 >= 0 && y2 >= 0 && y3 >= 0 && y4 >= 0)
            year = QString(strDt[y1]) + QString(strDt[y2]) + QString(strDt[y3]) + QString(strDt[y4]);
        
        
        if(dateAcceptable || QDate::fromString(QString("%1.%2.%3").arg(day).arg(month).arg(year), "dd.MM.yyyy").isValid())
            dateAcceptable = true;
        else {
            for(int i = 0; i < year.size(); i++)
                if(!year[i].isDigit() && year[i]!=tr("y")[0])
                    return State::Invalid;
            
            if(month.size() == 2){
                if(!month[1].isDigit() && month[1]!=tr("M")[0])
                    return State::Invalid;
                if(month[0].isDigit())
                {
                    int m0 = QString(month[0]).toInt();
                    if(m0 > 1)
                    {
                        str[M1] = '1';
                        str[M2] = '2';
                    }
                    
                    if(month[1].isDigit())
                    {
                        int m1 = QString(month[1]).toInt();
                        if(m0 == 0 && (m1 == 0))
                            str[M2] = '1';
                        if(m0 == 1 && (m1 < 0 || m1 > 2))
                            str[M2] = '2';
                    }
                    else if(month[1] != tr("M")[0])
                        return State::Invalid;
                    
                }
                else if(month[0] != tr("M")[0])
                    return State::Invalid;
            }
            
            if(day.size() == 2)
            {
                if(!day[1].isDigit() && day[1]!=tr("d")[0])
                    return State::Invalid;
                if(day[0].isDigit())
                {
                    int max0 = 3;
                    int max1 = 1;
                    
                    if(month[0].isDigit() && month[1].isDigit())
                    {
                        auto monthI = month.toInt();
                        
                        if(monthI==1 || monthI==3|| monthI==5 || monthI==7|| monthI==8 || monthI==10|| monthI==12)
                        {max0 = 3; max1 = 1;}//31
                        else if(monthI==4 || monthI==6|| monthI==9 || monthI==11)
                        {max0 = 3; max1 = 0;}//30
                        else if(monthI== 2)
                        {
                            max0 = 2; max1 = 9;
                            bool yearOk = false;
                            auto yearI = year.toInt(&yearOk);
                            if(yearOk && !QDate(yearI, monthI, 29).isValid())
                                max1 = 8;
                        }//28 or 29
                    }
                    
                    int d0 = QString(day[0]).toInt();
                    if(d0 < 0 || d0 > max0)
                    {str[d1] = QString("%1").arg(max0)[0]; str[d2] = QString("%1").arg(max1)[0];}//return State::Invalid;
                    
                    if(day[1].isDigit())
                    {
                        int d1 = QString(day[1]).toInt();
                        if(d0 == 0 && (d1 == 0))
                            str[d2]='1';//return State::Invalid;
                        if(d0 == max0 && (d1 < 0 || d1 > max1))
                            str[d2]=QString("%1").arg(max1)[0];//return State::Invalid;
                    }
                    else if(day[1] != tr("d")[0])
                        return State::Invalid;
                }
                else if(day[0] != tr("d")[0])
                    return State::Invalid;
            }
        }
        
        bool yearOk = false;
        auto yearI = year.toInt(&yearOk);
        if(yearOk) lastValidYear = year;
        
        bool monthOk = false;
        auto monthI = month.toInt(&monthOk);
        if(monthOk) lastValidMonth = month;
        
        bool dayOk = false;
        auto dayI = day.toInt(&dayOk);
        if(dayOk) lastValidDay = day;
        
        
        
        bool timeAcceptable = !(m_format.format.contains('h') || m_format.format.contains('m') || m_format.format.contains('s') ||
                                m_format.format.contains('z') || m_format.format.contains('f') );
        
        int h1 = -1, h2 = -1, m1 = -1, m2 = -1, s1 = -1, s2 = -1, z1 = -1, z2 = -1, z3 = -1, f1 = -1, f2 = -1;
        h1 = m_format.format.indexOf('h');
        if(h1 >= 0)
            h2 = m_format.format.indexOf('h', h1 + 1);
        m1 = m_format.format.indexOf('m');
        if(m1 >= 0)
            m2 = m_format.format.indexOf('m', m1 + 1);
        s1 = m_format.format.indexOf('s');
        if(s1 >= 0)
            s2 = m_format.format.indexOf('s', s1 + 1);
        z1 = m_format.format.indexOf('z');
        if(z1 >= 0)
            z2 = m_format.format.indexOf('z', z1 + 1);
        if(z2 >= 0)
            z3 = m_format.format.indexOf('z', z2 + 1);
        f1 = m_format.format.indexOf('f');
        if(f1 >= 0)
            f2 = m_format.format.indexOf('f', f1 + 1);
        
        QString hours, mins, secs, msecs, frames;
        if(h1 >= 0 && h2 >= 0)
            hours = QString(strDt[h1]) + QString(strDt[h2]);
        if(m1 >= 0 && m2 >= 0)
            mins = QString(strDt[m1]) + QString(strDt[m2]);
        if(s1 >= 0 && s2 >= 0)
            secs = QString(strDt[s1]) + QString(strDt[s2]);
        if(z1 >= 0 && z2 >= 0 && z3 > 0)
            msecs = QString(strDt[z1]) + QString(strDt[z2]) + QString(strDt[z3]);
        if(f1 >= 0 && f2 >= 0)
            frames = QString(strDt[f1]) + QString(strDt[f2]);
        
        
        if(frames.size() == 2)
        {
            if(m_format.frameRate <= 0)
            {
                if(!frames[0].isDigit() && frames[0]!=tr("f")[0])
                    return State::Invalid;
                if(!frames[1].isDigit() && frames[1]!=tr("f")[0])
                    return State::Invalid;
            }
            else
            {
                auto maxFrames = QString("%1").arg(m_format.frameRate - 1);
                if(maxFrames.size() == 1)
                    maxFrames.prepend('0');
                
                if(!frames[1].isDigit() && frames[1]!=tr("f")[0])
                    return State::Invalid;
                if(frames[0].isDigit())
                {
                    auto f0 = frames[0];
                    if(f0 > maxFrames[0])
                    {
                        str[f1] = maxFrames[0];
                        str[f2] = maxFrames[1];
                    }
                    else if(f0 == maxFrames[0] && frames[1].isDigit())
                    {
                        auto f1 = frames[1];
                        if(f1 > maxFrames[1])
                            str[f2] = maxFrames[1];
                    }
                }
                else if(frames[0]!=tr("f")[0])
                    return State::Invalid;
            }
        }
        
        bool framesOk;
        frames.toInt(&framesOk);
        if(!framesOk && !frames.isEmpty()) timeAcceptable = false;
        
        if(timeAcceptable ||
           ((QTime::fromString(QString("%1:%2:%3.%4").arg(hours).arg(mins).arg(secs).arg(msecs), "hh:mm:ss.zzz").isValid() ||
           QTime::fromString(QString("%1:%2:%3").arg(hours).arg(mins).arg(secs), "hh:mm:ss").isValid()) && (frames.isEmpty() || framesOk)))
            timeAcceptable = true;
        else {
            if(!msecs.isEmpty())
                for(int i = 0; i < msecs.size(); i++)
                    if(!msecs[i].isDigit() && msecs[i]!=tr("z")[0])
                        return State::Invalid;
            
            if(secs.size() == 2)
            {
                if(!secs[1].isDigit() && secs[1]!=tr("s")[0])
                    return State::Invalid;
                
                if(secs[0].isDigit())
                {
                    if(QString(secs[0]).toInt() > 5)
                    {
                        str[s1] = '5';
                        str[s2] = '9';
                    }
                }
                else if(secs[1]!=tr("s")[0])
                    return State::Invalid;
            }
            
            
            if(mins.size() == 2)
            {
                if(!mins[1].isDigit() && mins[1]!=tr("m")[0])
                    return State::Invalid;
                
                if(mins[0].isDigit())
                {
                    if(QString(mins[0]).toInt() > 5)
                    {
                        str[m1] = '5';
                        str[m2] = '9';
                    }
                }
                else if(mins[1]!=tr("m")[0])
                    return State::Invalid;
            }
            
            if(hours.size() == 2)
            {
                if(!hours[1].isDigit() && hours[1]!=tr("h")[0])
                    return State::Invalid;
                if(hours[0].isDigit())
                {
                    auto h0 = QString(hours[0]).toInt();
                    if(h0 > 2)
                    {
                        str[h1] = '2';
                        str[h2] = '3';
                    }
                    else if(h0 == 2 && hours[1].isDigit())
                    {
                        auto h1 = QString(hours[1]).toInt();
                        if(h1 > 3)
                            str[h2] = '3';
                    }
                }
                else if(hours[0]!=tr("h")[0])
                    return State::Invalid;
            }
        }
        
        bool hourOk = false;
        hours.toInt(&hourOk);
        if(hourOk) lastValidHour = hours;
        
        bool minuteOk = false;
        mins.toInt(&minuteOk);
        if(minuteOk) lastValidMinute = mins;
        
        bool secondsOk = false;
        secs.toInt(&secondsOk);
        if(secondsOk) lastValidSecond = secs;
        
        bool msecsOk = false;
        msecs.toInt(&msecsOk);
        if(msecsOk) lastValidMsOrFrames = msecs;
        else if(framesOk) lastValidMsOrFrames = frames;
        
        return (dateAcceptable && timeAcceptable) ? State::Acceptable : State::Intermediate;
    }
    
    void fixup(QString& strDt) const override
    {
        int d1 = -1, d2 = -1, M1 = -1, M2 = -1, y1 = -1, y2 = -1, y3 = -1, y4 = -1;
        d1 = m_format.format.indexOf('d');
        if(d1 >= 0)
            d2 = m_format.format.indexOf('d', d1 + 1);
        M1 = m_format.format.indexOf('M');
        if(M1 >= 0)
            M2 = m_format.format.indexOf('M', M1 + 1);
        y1 = m_format.format.indexOf('y');
        if(y1 >= 0)
            y2 = m_format.format.indexOf('y', y1 + 1);
        if(y2 >= 0)
            y3 = m_format.format.indexOf('y', y2 + 1);
        if(y3 >= 0)
            y4 = m_format.format.indexOf('y', y3 + 1);
        
        if(d1 >= 0 && d2 >= 0 && !lastValidDay.isEmpty())
        {
            strDt[d1] = lastValidDay[0];
            strDt[d2] = lastValidDay[1];
        }
        
        if(M1 >= 0 && M2 >= 0 && !lastValidMonth.isEmpty())
        {
            strDt[M1] = lastValidMonth[0];
            strDt[M2] = lastValidMonth[1];
        }
        
        if(y1 >= 0 && y2 >= 0 &&  y3 >= 0 && y4 > 0 && !lastValidYear.isEmpty())
        {
            strDt[y1] = lastValidYear[0];
            strDt[y2] = lastValidYear[1];
            strDt[y3] = lastValidYear[2];
            strDt[y4] = lastValidYear[3];
        }
        
        //time
        int h1 = -1, h2 = -1, m1 = -1, m2 = -1, s1 = -1, s2 = -1, z1 = -1, z2 = -1, z3 = -1, f1 = -1, f2 = -1;
        h1 = m_format.format.indexOf('h');
        if(h1 >= 0)
            h2 = m_format.format.indexOf('h', h1 + 1);
        m1 = m_format.format.indexOf('m');
        if(m1 >= 0)
            m2 = m_format.format.indexOf('m', m1 + 1);
        s1 = m_format.format.indexOf('s');
        if(s1 >= 0)
            s2 = m_format.format.indexOf('s', s1 + 1);
        z1 = m_format.format.indexOf('z');
        if(z1 >= 0)
            z2 = m_format.format.indexOf('z', z1 + 1);
        if(z2 >= 0)
            z3 = m_format.format.indexOf('z', z2 + 1);
        f1 = m_format.format.indexOf('f');
        if(f1 >= 0)
            f2 = m_format.format.indexOf('f', f1 + 1);
        
        if(h1 >= 0 && h2 >= 0 && !lastValidHour.isEmpty())
        {
            strDt[h1] = lastValidHour[0];
            strDt[h2] = lastValidHour[1];
        }
        if(m1 >= 0 && m2 >= 0 && !lastValidMinute.isEmpty())
        {
            strDt[m1] = lastValidMinute[0];
            strDt[m2] = lastValidMinute[1];
        }
        if(s1 >= 0 && s2 >= 0 && !lastValidSecond.isEmpty())
        {
            strDt[s1] = lastValidSecond[0];
            strDt[s2] = lastValidSecond[1];
        }
        if(z1 >= 0 && z2 >= 0 && z3 >= 0 && !lastValidMsOrFrames.isEmpty())
        {
            strDt[z1] = lastValidMsOrFrames[0];
            strDt[z2] = lastValidMsOrFrames[1];
            strDt[z3] = lastValidMsOrFrames[2];
        }
        if(f1 >= 0 && f2 >= 0 && !lastValidMsOrFrames.isEmpty())
        {
            strDt[f1] = lastValidMsOrFrames[0];
            strDt[f2] = lastValidMsOrFrames[1];
        }
    }
};

void CustomDateTimeEdit::ShowCal()
{
    m_calVisible = true;
    if(m_cal && m_cal->isHidden())
        m_cal->show();
}

void CustomDateTimeEdit::HideCal()
{
    m_calVisible = false;
    if(m_cal && !m_cal->isHidden())
        m_cal->hide();
}

CustomDateTimeEdit::CustomDateTimeEdit(QWidget *parent) :
    QLineEdit(parent)
{
    qRegisterMetaType<CustomDateTimeEdit::Format>();
    
    m_lineEdit = this;
    //раскомментить это и закомментить выше если все-таки захочется переделать на :QFrame
    //и тогда еще внутрянку keyPressEvent надо перенести в eventFilter и поставить его на внутренний lineEdit
    //m_lineEdit = new QLineEdit(this);
    //auto lay = new QVBoxLayout(this);
    //this->setLayout(lay);
    //lay->addWidget(m_lineEdit);
    //lay->setContentsMargins(0, 0, 0, 0);
    //lay->setSpacing(0);
    //this->setFocusProxy(m_lineEdit);
    
    setFormat("dd.MM.yyyy hh:mm:ss");
    
    QWidget* parentWgtIter = this;
    m_parentChangeWatcher = new _CustomDateTimeEditParentChangeWatcher(this);
    while(parentWgtIter)
    {
        parentWgtIter->installEventFilter(m_parentChangeWatcher);
        parentWgtIter = parentWgtIter->parentWidget();
    }
    connect(m_parentChangeWatcher, &_CustomDateTimeEditParentChangeWatcher::parentChanged, this, &CustomDateTimeEdit::onTopLevelWidgetChanged);
    
    connect(m_lineEdit, &QLineEdit::textChanged, this, [this](const QString& text)
            {
                auto f = m_format.format;
                QList<int> indexesOfF;
                for(int i = f.size() - 1; i >= 0; i--)
                    if(f[i] == 'f')
                    {
                        indexesOfF.append(i);
                        f.remove(i, 1);
                    }
                auto str = text;
                QString framesStr;
                for(auto i : indexesOfF)
                {
                    framesStr.prepend(str[i]);
                    str.remove(i, 1);
                }
                auto dateTime = QDateTime::fromString(str, f);
                if(dateTime.isValid())
                {
                    m_dateTime = dateTime;
                    m_frames = framesStr.toInt();
                    emit dateTimeChanged(m_dateTime);
                    emit dateTimeCodeChanged(dateTimeCode());
                }
            });
    
    connect(m_lineEdit, &QLineEdit::selectionChanged, this, [this]()
            {
                m_lineEdit->deselect();
            });
    connect(m_lineEdit, &QLineEdit::cursorPositionChanged, this, [this](int oldPos, int newPos)
            {
                m_currentSection = newPos;
                int maxPos = m_format.format.length() - 1;
                if(newPos > maxPos)
                    m_lineEdit->setCursorPosition(maxPos);
            });
    
    connect(m_lineEdit, &QLineEdit::returnPressed, this, [this](){
        emit editingFinished();
    });
    
    setCalendarShown(m_calendarShown);
}

void CustomDateTimeEdit::onTopLevelWidgetChanged()
{
    delete m_parentChangeWatcher;
    m_parentChangeWatcher = nullptr;
    if(m_cal)
    {
        HideCal();
        qApp->processEvents();
        onFocusChanged(nullptr, qApp->focusWidget());
    }
    
    QWidget* parentWgtIter = this;
    m_parentChangeWatcher = new _CustomDateTimeEditParentChangeWatcher(this);
    while(parentWgtIter)
    {
        parentWgtIter->installEventFilter(m_parentChangeWatcher);
        parentWgtIter = parentWgtIter->parentWidget();
    }
    
    connect(m_parentChangeWatcher, &_CustomDateTimeEditParentChangeWatcher::parentChanged, this, &CustomDateTimeEdit::onTopLevelWidgetChanged);
}

CustomDateTimeEdit::~CustomDateTimeEdit()
{
    delete m_parentChangeWatcher;
    if(m_cal)
    {
        QObject::disconnect(m_calConn);
        delete m_cal;
    }
}

QDate CustomDateTimeEdit::date()
{
    return m_dateTime.date();
}

void CustomDateTimeEdit::setDate(QDate date)
{
    m_dateTime.setDate(date);
    {QSignalBlocker bl{m_lineEdit};
        m_lineEdit->setText(getAsString());
        setCurrentSection(m_currentSection);
    }
    emit dateTimeChanged(m_dateTime);
}

QTime CustomDateTimeEdit::time()
{
    return m_dateTime.time();
}

void CustomDateTimeEdit::setTime(QTime time)
{
    if(!m_dateTime.isValid())
        m_dateTime = QDateTime::currentDateTime();
    m_dateTime.setTime(time);
    {QSignalBlocker bl{m_lineEdit};
        m_lineEdit->setText(getAsString());
        setCurrentSection(m_currentSection);
    }
    emit dateTimeChanged(m_dateTime);
}

QDateTime CustomDateTimeEdit::dateTime()
{
    return QDateTime(date(), time());
}

void CustomDateTimeEdit::setDateTime(QDateTime dt)
{
    m_dateTime = dt;
    {QSignalBlocker bl{m_lineEdit};
        m_lineEdit->setText(getAsString());
        setCurrentSection(m_currentSection);
    }
    emit dateTimeChanged(dt);
}

DateTimeCode CustomDateTimeEdit::dateTimeCode()
{
    DateTimeCode dt;
    dt.SetDateTime(m_dateTime);
    dt.SetFrames(m_frames);
    return dt;
}

void CustomDateTimeEdit::setDateTimeCode(DateTimeCode dt)
{
    m_dateTime = dt.DateTime();
    m_frames = dt.Frames();
    {QSignalBlocker bl{m_lineEdit};
        m_lineEdit->setText(getAsString());
        setCurrentSection(m_currentSection);
    }
    emit dateTimeCodeChanged(dt);
}

int CustomDateTimeEdit::currentSection()
{
    return m_currentSection;
}

void CustomDateTimeEdit::setCurrentSection(int section)
{
    m_lineEdit->setCursorPosition(section);
}

void CustomDateTimeEdit::setFormat(const CustomDateTimeEdit::Format& format)
{
    m_format = format;
    
    m_lineEdit->setValidator(nullptr);
    
    QString inputMask;
    for(int i = 0; i < format.format.size(); i++)
    {
        auto ch = format.format[i];
        if(ch == 'h')
            inputMask += 'N';
        else if(ch == 'm')
            inputMask += 'N';
        else if(ch == 's')
            inputMask += 'N';
        else if(ch == 'z')
            inputMask += 'N';
        else if(ch == 'f')
            inputMask += 'N';
        else if(ch == 'd')
            inputMask += 'N';
        else if(ch == 'M')
            inputMask += 'N';
        else if(ch == 'y')
            inputMask += 'N';
        else //separator
            inputMask += ch;
    }
    
    m_lineEdit->setInputMask(inputMask);
    m_lineEdit->setValidator(new DateTimeValidator(m_format));
    this->setMinimumWidth(this->fontMetrics().horizontalAdvance(m_format.format));
    
    //чтобы обновить попап, если вдруг в новом формате появилась/пропала дата
    setCalendarShown(m_calendarShown);
    
    emit formatChanged(m_format);
}

QString CustomDateTimeEdit::getAsString(const CustomDateTimeEdit::Format& format)
{
    auto f = format.format.isEmpty() ? m_format : format;
    return DateTimeToString(m_dateTime, m_frames, f);
}


void CustomDateTimeEdit::fillSymbolsWithNumber(QString& str, QChar symbol, int number)
{
    int sCount = 0;
    for(int i = 0; i < str.size(); i++)
        if(str[i] == symbol)
            sCount++;
    
    if(sCount == 0)
        return;
    
    auto numStr = QString::number(number);
    //добить начало строки нулями если символов больше чем знаков
    while(numStr.size() < sCount)
        numStr.prepend('0');
    
    //расставить знаки на места символов
    int indOfF = -1, lastIndexOfF = -1, cnt = 0;
    while((indOfF = str.indexOf(symbol, indOfF+1)) != -1 && cnt < sCount)
    {
        str[indOfF] = numStr[cnt++];
        lastIndexOfF = indOfF;
    }
    
    //добить остальные цифры кадров если в формате было меньше символов
    while(cnt < numStr.size())
        str.insert(++lastIndexOfF, numStr[cnt++]);
}

QString CustomDateTimeEdit::DateTimeToString(const QDateTime& dt, int frames, const CustomDateTimeEdit::Format& format)
{
    auto dateTime = dt;
    auto f = format.format;
    int fsCount  = 0;
    bool skip = false;
    for(int i = 0; i < f.size(); i++)
    {
        if(f[i]=='\'')
            skip = !skip;
        
        if(!skip && f[i] == 'f')
        {
            fsCount++;
            if(i-1 >= 0 && f[i-1] != 'f')
            {
                f.insert(i, '\'');
                i++; 
            }
            
            if(i+1 == f.size() || f[i+1] != 'f')
            {
                f.insert(i+1, '\'');
                i++;
            }
        }
    }
    
    if(format.frameRate > 0)
        dateTime = dateTime.addSecs(frames/format.frameRate);
    
    auto str = dateTime.toString(format.format);
    fillSymbolsWithNumber(str, 'f', format.frameRate > 0 ? frames%format.frameRate : frames);
    
    return str;
}

QString CustomDateTimeEdit::DurationToString(const DateTimeCode& d2, const DateTimeCode& d1, const Format& format)
{
    //пока поддерживаются только часы
    Q_ASSERT(!format.format.toLower().contains('d'));
    Q_ASSERT(!format.format.contains('M'));
    Q_ASSERT(!format.format.toLower().contains('y'));
    
    bool d2larger = !(d2 < d1);
    
    auto dt1 = d2larger ? d1.DateTime() : d2.DateTime();
    auto dt2 = d2larger ? d2.DateTime() : d1.DateTime();
    auto t1 = dt1.time(); auto t2 = dt2.time();
    t1.setHMS(t1.hour(), t1.minute(), t1.second(), 0); t2.setHMS(t2.hour(), t2.minute(), t2.second(), 0);
    dt1.setTime(t1); dt2.setTime(t2);
    
    auto frameDiff = d2larger ? d2.Frames() - d1.Frames() : d1.Frames() - d2.Frames();
    auto secsDiff = dt1.secsTo(dt2);
    if(frameDiff < 0)
    {
        secsDiff--;
        frameDiff = format.frameRate + frameDiff;
    }
    
    bool otricatelnoe = secsDiff < 0;
    if(otricatelnoe)
        secsDiff = -secsDiff;
            
    auto hours = secsDiff / 3600;
    auto minutes = (secsDiff % 3600) / 60;
    auto seconds = secsDiff % 60;
    auto frames = frameDiff;
    
    auto str = format.format;
    fillSymbolsWithNumber(str, 'h', hours);
    fillSymbolsWithNumber(str, 'm', minutes);
    fillSymbolsWithNumber(str, 's', seconds);
    fillSymbolsWithNumber(str, 'f', frames);
    if(!d2larger)
        otricatelnoe = !otricatelnoe;
    if(otricatelnoe)
        str.prepend('-');
    
    return str;
}

QString CustomDateTimeEdit::DurationToString(int64_t frameDiff, const Format& format)
{
    bool otric = frameDiff < 0;
    if(otric)
        frameDiff = -frameDiff;
    
    auto secsDiff = frameDiff / format.frameRate;
    frameDiff = frameDiff % format.frameRate;
    
    auto hours = secsDiff / 3600;
    auto minutes = (secsDiff % 3600) / 60;
    auto seconds = secsDiff % 60;
    auto frames = frameDiff;
    
    auto str = format.format;
    fillSymbolsWithNumber(str, 'h', hours);
    fillSymbolsWithNumber(str, 'm', minutes);
    fillSymbolsWithNumber(str, 's', seconds);
    fillSymbolsWithNumber(str, 'f', frames);
    
    if(otric)
        str.prepend('-');
    
    return str;
}

int CustomDateTimeEdit::lastSection()
{
    return m_format.format.length() - 1;
}

QWidget* CustomDateTimeEdit::FocusProxy()
{
    return this;
}

void CustomDateTimeEdit::setCalendarShown(bool show)
{
    if(show && m_format.HasDate())
    {
        if(m_cal)
            return;
        
        {QSignalBlocker blocker{qApp};
            m_cal = new CustomCalendar(nullptr, this);
            
            m_cal->setAttribute(Qt::WA_ShowWithoutActivating);
            m_cal->setWindowFlags(Qt::Tool | Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
        }
        
        m_cal->SetSelectedSideStylesheet("");
        m_cal->setFocusPolicy(Qt::StrongFocus);
        m_cal->setAutoFillBackground(true);
        
        m_cal->SetIsRange(false);
        m_cal->SetHideOnOk(true);
        HideCal();
        
        m_calConn = connect(qApp, &QApplication::focusChanged, this, &CustomDateTimeEdit::onFocusChanged);
        
        qApp->processEvents(); //чтобы далее позиции были верны
        onFocusChanged(nullptr, qApp->focusWidget());
    }
    else
    {
        if(m_cal)
        {
            QObject::disconnect(m_calConn);
            m_cal->deleteLater();
        }
        m_cal = nullptr;
    }
    m_calendarShown = show;
}

bool CustomDateTimeEdit::TestFocus(QWidget* wgt)
{
    return isChildOf(wgt, this) || isChildOf(wgt, m_cal);
}

void CustomDateTimeEdit::UpdatePopupGeometry()
{
    if(m_cal)
    {
        auto position = QWidget::mapToGlobal(this->rect().bottomLeft());
        position.setY(position.y() + 9);
        
        m_cal->setGeometry(position.x(),
                           position.y(),
                           m_cal->size().width(),
                           m_cal->size().height());
        
        if(m_calVisible)
            ShowCal();
    }
}

void CustomDateTimeEdit::onFocusChanged(QWidget* old, QWidget* now)
{
    bool testFocus = TestFocus(now);
    if(m_cal && m_cal->isHidden() && testFocus)
    {
        UpdatePopupGeometry();
        
        ShowCal();
    }
    else if(testFocus == false)
        HideCal();
}

void CustomDateTimeEdit::keyPressEvent(QKeyEvent* event)
{
    if( event->key() == Qt::Key_0 ||
        event->key() == Qt::Key_1 ||
        event->key() == Qt::Key_2 ||
        event->key() == Qt::Key_3 ||
        event->key() == Qt::Key_4 ||
        event->key() == Qt::Key_5 ||
        event->key() == Qt::Key_6 ||
        event->key() == Qt::Key_7 ||
        event->key() == Qt::Key_8 ||
        event->key() == Qt::Key_9 ||
        event->key() == Qt::Key_Left ||
        event->key() == Qt::Key_Right ||
        event->key() == Qt::Key_Return ||
        event->key() == Qt::Key_Enter ||
        event->key() == Qt::Key_End ||
        event->key() == Qt::Key_Home ||
        event->key() == Qt::Key_Escape)
    {
        if(currentSection() == lastSection() && (event->key() == Qt::Key_Right ||
                                                  event->key() == Qt::Key_0 ||
                                                  event->key() == Qt::Key_1 ||
                                                  event->key() == Qt::Key_2 ||
                                                  event->key() == Qt::Key_3 ||
                                                  event->key() == Qt::Key_4 ||
                                                  event->key() == Qt::Key_5 ||
                                                  event->key() == Qt::Key_6 ||
                                                  event->key() == Qt::Key_7 ||
                                                  event->key() == Qt::Key_8 ||
                                                  event->key() == Qt::Key_9))
            emit movedToLastSection();
        if(currentSection() == 0 && event->key() == Qt::Key_Left)
            emit movedToTheLeft();
        if(event->key() == Qt::Key_Escape)
            emit editingCanceled();
        Base::keyPressEvent(event);
    }   
    
    if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
    {
        return;
        //code for replacing chars with format symbols??
    }
    
    if(event->modifiers().testFlag(Qt::ControlModifier) &&
        (event->key() == Qt::Key_A ||
         event->key() == Qt::Key_C ||
         event->key() == Qt::Key_V))
        Base::keyPressEvent(event);
}

bool CustomDateTimeEdit::eventFilter(QObject* watched, QEvent* event)
{
    return Base::eventFilter(watched, event);
}

bool CustomDateTimeEdit::event(QEvent* event)
{
    if(event->type() == QEvent::FocusIn)
    {
        repaint(); //иначе cursorPositionAt не учитывает выравнивание и считает какбудто оно по умолчанию левое
        int cursorPos = m_lineEdit->cursorPositionAt(m_lineEdit->mapFromGlobal(cursor().pos()));
        if(cursorPos >= 0 && cursorPos <= m_format.format.length()-1)
            m_lineEdit->setCursorPosition(cursorPos);
    }
    
    if(m_cal && (event->type() == QEvent::Resize || event->type() == QEvent::Move))
    {
        auto pos = this->pos();
        auto mappedPos = QWidget::mapTo(m_cal->parentWidget(), pos);
        if(mappedPos != m_lastPosition)
            UpdatePopupGeometry();
        m_lastPosition = mappedPos;
    }
    return Base::event(event);
}

bool _CustomDateTimeEditParentChangeWatcher::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::ParentChange)
        emit parentChanged();
    
    if((event->type() == QEvent::Move || event->type() == QEvent::Resize) && m_edit->m_cal)
    {
        auto pos = m_edit->pos();
        auto mappedPos = m_edit->mapToGlobal(pos);
        if(mappedPos != m_edit->m_lastPosition || event->type() == QEvent::Resize)
            m_edit->UpdatePopupGeometry();
        m_edit->m_lastPosition = mappedPos;
    }
    
    return false;
}
