#ifndef CUSTOMINTERVALEDIT_H
#define CUSTOMINTERVALEDIT_H

#include <QFrame>
#include <QSpacerItem>
#include <QLabel>
#include <CustomCalendar.h>

namespace Ui {
class CustomIntervalEdit;
}

class _CustomIntervalEditParentChangeWatcher;
class CustomIntervalEdit : public QFrame
{
    Q_OBJECT
    
    CustomCalendar* m_cal = nullptr;
    bool m_showCal = false;
    QWidget* m_parent = nullptr;
    QWidget* m_lastFocusedWidget = nullptr;
    
    friend class _CustomIntervalEditParentChangeWatcher;
    _CustomIntervalEditParentChangeWatcher* m_parentChangeWatcher = nullptr;
    
    bool isChildOf(QWidget* child, QWidget* parent)
    {
        while(child) {
            if(child == parent)
                return true;
            child = child->parentWidget();
        }
        return false;
    }
public:
    explicit CustomIntervalEdit(QWidget *parent = nullptr);
    ~CustomIntervalEdit();
    
    QDateTime In();
    QDateTime Out();
    
    void SetIn(QDateTime in);
    void SetOut(QDateTime out);
    
    CustomDateTimeEdit* InEdit();
    CustomDateTimeEdit* OutEdit();

    QLabel* LabelDelimiter();
    QFrame* backgroundFrame();
    
    bool IsCalendarPopupShown() { return m_showCal; }
    void SetCalendarPopupShown(bool show);
    
    bool TestFocus(QWidget* wgt);

    void setHWidgetAlignment(Qt::Alignment alig, bool needRightIcon = false);
    
    QWidget* FocusProxy();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::CustomIntervalEdit *ui;

    //Для Alignment
    Qt::Alignment curAlig;
    bool needOutEditRightIcon = false;
    QSpacerItem* leftSpacer = nullptr;
    QSpacerItem* rightSpacer = nullptr;

    //Для OutEdit без QTime shown
    void setMaxWidthOutInterval(int width);
    const int defaultmaxOutIntervalWidth = 80;
    int maxOutIntervalWidth = 80;


private slots:
    void onTopLevelWidgetChanged();
    void onFocusChanged(QWidget* old, QWidget* now);
protected:
    void focusInEvent(QFocusEvent* event) override;
signals:
    void editingFinished();
    void lostFocus();
    void editingCanceled();
};

class _CustomIntervalEditParentChangeWatcher : public QObject
{
    Q_OBJECT
public:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if(event->type() == QEvent::ParentChange)
            emit parentChanged();
        return false;
    }
signals:
    void parentChanged();
};

#endif // CUSTOMINTERVALEDIT_H
