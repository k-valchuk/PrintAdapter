#ifndef CUSTOMDATETIMEEDIT_H
#define CUSTOMDATETIMEEDIT_H

#include "qcoreevent.h"
#include "qdatetime.h"
#include <QFrame>
#include <QLineEdit>
#include <QRegularExpression>

class DateTimeCode
{
    QDateTime m_dateTime;
    int m_frames = 0;
    
public:
    inline DateTimeCode();
    
    DateTimeCode(const QDateTime& dt, int framerate = 0)
        : DateTimeCode()
    {
        SetDateTime(dt, framerate);
    }
    
    void SetDateTime(const QDateTime& dt, int framerate = 0)
    {
        m_dateTime = dt;
        auto time = dt.time(); time.setHMS(time.hour(), time.minute(), time.second(), framerate != 0 ? 0 : time.msec());
        m_dateTime.setTime(time);
        auto msecs = dt.time().msec();
        if(framerate != 0)
        {
            m_frames = qRound(msecs/(1000.0/(double)framerate));
            if(m_frames > 0 && m_frames == framerate)
            {
                m_frames = 0;
                m_dateTime = m_dateTime.addSecs(1);
            }
        }
    }
    QDateTime DateTime() const {return m_dateTime;}
    QDateTime DateTime(int framerate) const {
        QDateTime res = m_dateTime;
        auto time = res.time();
        auto msecs = qRound(m_frames * (1000.0/(double)framerate));
        time.setHMS(time.hour(), time.minute(), time.second(), msecs);
        res.setTime(time);
        return res;
    }
    void SetFrames(int frames) {m_frames = frames;}
    int Frames() const {return m_frames;}
    
    bool operator<(const DateTimeCode& other) const
    {
        if(m_dateTime != other.m_dateTime)
            return m_dateTime < other.m_dateTime;
        else
            return m_frames < other.m_frames;
    }
    bool operator>(const DateTimeCode& other) const
    {
        if(m_dateTime != other.m_dateTime)
            return m_dateTime > other.m_dateTime;
        else
            return m_frames > other.m_frames;
    }
    bool operator==(const DateTimeCode& other) const
    {
        return m_dateTime == other.m_dateTime && m_frames == other.m_frames;
    }
};

Q_DECLARE_METATYPE(DateTimeCode);
DateTimeCode::DateTimeCode()
{
    qRegisterMetaType<DateTimeCode>();
}

class CustomCalendar;

class _CustomDateTimeEditParentChangeWatcher;
class CustomDateTimeEdit : public QLineEdit
{
    Q_OBJECT
public:
    struct Format
    {
        QString format;
        int frameRate = -1;
        
        Format(){}
        Format(const QString& f){
            format = f;
            if(format.contains(m_frameRateRegExp))
            {
                auto match = m_frameRateRegExp.match(format);
                auto strFrameRate = match.captured();
                format.remove(m_frameRateRegExp);
                
                frameRate = strFrameRate.mid(1).toInt();
            }
        }
        Format(const char* f) : Format(QString(f)){}
        Format(const QString& f, int framerate) : Format(f){
            frameRate = framerate;
        }
        
        bool HasDate()
        {
            return format.contains("d") || format.contains("M") || format.contains("Y");
        }
    private:
        QRegularExpression m_frameRateRegExp{"\\/\\d+"};
    };
private:
    using Base = QLineEdit;
    
    int m_currentSection = 0;
    Format m_format;
    bool m_calendarShown = false;
    bool m_allowErasing = false;
    CustomCalendar* m_cal = nullptr;
    QPoint m_lastPosition;
    QMetaObject::Connection m_calConn;
    QLineEdit* m_lineEdit;
    
    QDateTime m_dateTime; int m_frames = 0;
    
    friend class _CustomDateTimeEditParentChangeWatcher;
    _CustomDateTimeEditParentChangeWatcher* m_parentChangeWatcher = nullptr;
    
    bool isChildOf(QWidget* child, QWidget* parent)
    {
        if(parent == nullptr) return false;
        while(child) {
            if(child == parent)
                return true;
            child = child->parentWidget();
        }
        return false;
    }
    
    bool m_calVisible = false;
    void ShowCal();
    void HideCal();
    
    static void fillSymbolsWithNumber(QString& str, QChar symbol, int number);
public: 
    explicit CustomDateTimeEdit(QWidget *parent = nullptr);
    ~CustomDateTimeEdit();
    
    QDate date();
    void setDate(QDate date);
    QTime time();
    void setTime(QTime time);
    QDateTime dateTime();
    void setDateTime(QDateTime dt);
    
    DateTimeCode dateTimeCode();
    void setDateTimeCode(DateTimeCode dt);
    
    int currentSection();
    void setCurrentSection(int section);
    
    CustomDateTimeEdit::Format format() {return m_format;}
    void setFormat(const CustomDateTimeEdit::Format& format);
    
    QString getAsString(const CustomDateTimeEdit::Format& format = {});
    static QString DateTimeToString(const QDateTime& dt, int frames, const CustomDateTimeEdit::Format& format);
    static QString DurationToString(const DateTimeCode& dt2, const DateTimeCode& dt1, const CustomDateTimeEdit::Format& format);
    static QString DurationToString(int64_t framesDiff, const CustomDateTimeEdit::Format& format);
    
    int lastSection();
    QWidget* FocusProxy();
    
    bool isCalendarShown() { return m_cal != nullptr; }
    void setCalendarShown(bool show);
    bool TestFocus(QWidget* wgt);
    void UpdatePopupGeometry();
    
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
private slots:
    void onFocusChanged(QWidget* old, QWidget* now);
    void onTopLevelWidgetChanged();
    
signals:
    void dateTimeChanged(QDateTime dt);
    void dateTimeCodeChanged(DateTimeCode dt);
    void movedToLastSection();
    void movedToTheLeft();
    void editingFinished();
    void editingCanceled();
    void formatChanged(const CustomDateTimeEdit::Format& format);
    
public:
    void keyPressEvent(QKeyEvent* event) override;
    bool event(QEvent* event) override;
};

Q_DECLARE_METATYPE(CustomDateTimeEdit::Format);

class _CustomDateTimeEditParentChangeWatcher : public QObject
{
    Q_OBJECT
    
    CustomDateTimeEdit* m_edit;
public:
    _CustomDateTimeEditParentChangeWatcher(CustomDateTimeEdit* edit) : m_edit(edit) {}
    
    bool eventFilter(QObject* watched, QEvent* event) override;
signals:
    void parentChanged();
};

#endif // CUSTOMDATETIMEEDIT_H
