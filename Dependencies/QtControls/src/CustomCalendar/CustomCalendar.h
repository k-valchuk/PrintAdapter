#ifndef CUSTOMCALENDAR_H
#define CUSTOMCALENDAR_H

#include "qapplication.h"
#include "qevent.h"
#include "qpainter.h"
#include "qtextformat.h"
#include <QFrame>
#include <QDateTime>
#include <QPushButton>
#include <QCalendarWidget>
#include <optional>

namespace Ui {
class CustomCalendar;
}


class CustomDateTimeEdit;
class CustomCalendar : public QFrame
{
    Q_OBJECT
    
    enum Mode {
        Days,
        Months,
        Years
    };
    
    bool m_hideOnOk = true;
    Mode m_currentMode = Mode::Days;
    QPushButton* m_btns[12];
    int m_btnsData[12];
    
    bool m_isRange = false;
    bool m_editingIn = true;
    bool m_scrollCalendarOnEditorsFocus = true;
    
    bool m_hideIn = false;
    bool m_hideOut = false;
    QString GetMonthName(int month);
    QString GetShortMonthName(int month);
    
    QString m_selectedBorderStyle = "border: 1px solid palette(highlight); border-radius: 2px;";
    QString m_baseEditStyle;
    
    void UpdateButtonsStyles();
    
    void SetMode(Mode mode, bool setSection = true);
    void SetEditingSide(bool editingIn);
    
    bool m_onCalendarPage = true;
    void SetCurrentIndex(bool onCalendarPage);
    
    std::optional<QDateTime> m_minDtIn  = std::nullopt, m_maxDtIn = std::nullopt;
    std::optional<QDateTime> m_minDtOut = std::nullopt, m_maxDtOut = std::nullopt;
    static bool coerceDt(QDateTime& dt, const std::optional<QDateTime>& min, const std::optional<QDateTime>& max);
public:
    explicit CustomCalendar(QWidget *parent = nullptr, CustomDateTimeEdit* in = nullptr, CustomDateTimeEdit* out = nullptr);
    ~CustomCalendar();
    
    QDateTime InDateTime();
    QDateTime OutDateTime();

    CustomDateTimeEdit* InDateTimeEdit();
    
    void SetInDateTime(const QDateTime& dt);
    void SetOutDateTime(const QDateTime& dt);
    
    void SetInMinMaxDateTimes(const std::optional<QDateTime>& min, const std::optional<QDateTime>& max);
    void SetOutMinMaxDateTimes(const std::optional<QDateTime>& min, const std::optional<QDateTime>& max);
    
    void SetHideOnOk(bool hideOnOk);
    void SetOkBtnVisible(bool visible);
    void SetIsRange(bool isRange);
    bool IsRange() {return m_isRange;}
    
    void SetSelectedSideStylesheet(const QString& style);
    void SetScrollCalendarOnEditorsFocus(bool enable) {m_scrollCalendarOnEditorsFocus = enable;}
private:
    Ui::CustomCalendar *ui;
    
signals:
    void okPressed(QDateTime in, QDateTime out);
    void datesChanged(QDateTime in, QDateTime out);
    
protected:
    void showEvent(QShowEvent* event) override;
};

class ScrollableCalendarFrame : public QFrame
{
    Q_OBJECT
    
public:
    ScrollableCalendarFrame(QWidget* parent) : QFrame(nullptr) {}
    
    
protected:
    void wheelEvent(QWheelEvent* event) override
    {
        int cnt = event->angleDelta().ry()/120;
        if(cnt > 0)
            for(int i = 0; i < cnt; i++)
                emit scrolledUp();
        else if(cnt < 0)
            for(int i = 0; i < -cnt; i++)
                emit scrolledDown();
    }
    
signals:
    void scrolledUp();
    void scrolledDown();
};

class CustomCalendarWidget : public QCalendarWidget {
    Q_OBJECT
    
    CustomCalendar* m_customCalendar = nullptr;
public:
    CustomCalendarWidget(QWidget* parent = nullptr)
        : QCalendarWidget(parent)
    {
        this->setWeekdayTextFormat(Qt::Saturday, this->weekdayTextFormat(Qt::Monday));
        this->setWeekdayTextFormat(Qt::Sunday, this->weekdayTextFormat(Qt::Monday));
        
        auto headerTextFormat = this->headerTextFormat();
        headerTextFormat.setForeground(QApplication::palette().color(QPalette::ColorRole::HighlightedText));
        this->setHeaderTextFormat(headerTextFormat);
    }
    
    void SetParentCustomCalendar(CustomCalendar* customCalendar)
    {
        m_customCalendar = customCalendar;
        if(m_customCalendar)
            connect(m_customCalendar, &CustomCalendar::datesChanged, [this](QDateTime in, QDateTime out){
                this->update();
            });
    }
    
protected:
    void paintCell(QPainter* painter, const QRect& originalRect, const QDate& date) const override
    {
        auto rect = originalRect;
        if(m_customCalendar)
        {
            auto in = m_customCalendar->InDateTime().date();
            auto out = m_customCalendar->OutDateTime().date();
            
            bool wasDrawn = false;
            
            auto todayPen = painter->pen();
            auto selectedPen = painter->pen();
            auto originalPen = painter->pen();
            todayPen.setColor(QColor::fromRgb(255, 255, 255));
            selectedPen.setColor(QColor::fromRgb(255, 255, 255));
            
            const double m = 0.075;
            const double r = 1.0 - 2*m;
            auto width = rect.width();
            auto height = rect.height();
            auto offsetX = qRound(width*m);
            auto offsetY = qRound(height*m);
            
            auto corner = rect.topLeft();
            
            if(date > in && date < out && m_customCalendar->IsRange())
            {
                if(date.addDays(-1) == in)
                    rect.setRect(corner.x() - offsetX-1, corner.y() + offsetY, width + offsetX+1, height*r);
                else if(date.addDays(1) == out)
                    rect.setRect(corner.x(), corner.y() + offsetY, width + offsetX, height*r);
                else
                    rect.setRect(corner.x(), corner.y() + offsetY, width, height*r);
                
                //painter->setPen(selectedPen);
                painter->fillRect(rect, QColor::fromRgb(69, 106, 158, 77));
                wasDrawn = true;
            }
            if(date == in || (date == out && m_customCalendar->IsRange()))
            {
                rect.setRect(corner.x() + offsetX, corner.y() + offsetY, width*r, height*r);
                
                painter->setPen(selectedPen);
                painter->fillRect(rect, QColor::fromRgb(69, 106, 158));
                auto pen = painter->pen();
                painter->setPen(QPen(QColor::fromRgb(69, 106, 158), 1.5, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap));
                painter->drawRect(rect);
                painter->setPen(pen);
                wasDrawn = true;
            }
            
            if(wasDrawn || date == QDate::currentDate())
            {
                if(date == QDate::currentDate())
                {
                    rect.setRect(corner.x() + offsetX, corner.y() + offsetY, width*r, height*r);
                    auto pen = painter->pen();
                    painter->setPen(QPen(QColor::fromRgb(69, 106, 158), 1.5, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap));
                    painter->drawRect(rect);
                    painter->setPen(pen);
                }
                
                if(date == QDate::currentDate())
                    painter->setPen(todayPen);
                painter->drawText(originalRect, Qt::AlignCenter | Qt::AlignHCenter,
                                  QString::number(date.day()));
                painter->setPen(originalPen);
                
                return;
            }
        }
        
        QCalendarWidget::paintCell(painter, rect, date); 
    }
};

#endif // CUSTOMCALENDAR_H
