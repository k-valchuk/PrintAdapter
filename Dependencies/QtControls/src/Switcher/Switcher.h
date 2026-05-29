
#ifndef SWITCHER_H
#define SWITCHER_H

#include "qeasingcurve.h"
#include <QFrame>
#include <QVariant>
#include <QObject>
#include <QPushButton>
#include <QApplication>
#include <QHBoxLayout>
#include <QStackedLayout>

class Switcher : public QFrame
{
    Q_OBJECT
    
public:
    struct Item
    {
        QString text;
        QVariant data;
        QColor bgColor   = QApplication::palette().color(QPalette::Highlight);
        QColor textColor = QApplication::palette().color(QPalette::BrightText);
    };
    
private:
    QList<Item> m_items;
    QList<QPushButton*> m_buttons;
    QPushButton* m_prevBtn = nullptr;
    int m_currentIndex = -1;
    
    QStackedLayout* m_stackedLayout;
    QBoxLayout* m_hLayout;
    QFrame m_container;
    class SwitcherEllipseFrame* m_ellipseContainer;
    
    int m_animationTime = 350;
    QEasingCurve m_easingCurve = QEasingCurve::Type::InOutQuad;
    
    bool m_onlyProgrammaticalChange = false;
        
    void UpdateEllipse(bool animation = true);
    void ModifyButton(QPushButton* btn, Item item);
    
    QWidget* m_installedOn = nullptr;
protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
public:
    Switcher(QWidget* parent = nullptr);
    ~Switcher();
    
    Item ItemAt(int index);
    QVariant DataAt(int index) {return ItemAt(index).data;}
    int Count() {return m_items.size();}
    int CurrentIndex() {return m_currentIndex;}
    
public slots:
    void InsertItem(int index, Item item);
    void AddItem(Item item) {InsertItem(-1, item);}
    void RemoveItemAt(int index);
    void EditItemAt(int index, Item nItem);
    void SetCurrentIndex(int index);
    void SetCurrentItemByData(const QVariant& data);
    
    void SetAnimationTime(int time) {m_animationTime = time;}
    void SetAnimationEasingCurve(const QEasingCurve& curve) {m_easingCurve = curve;}
    void SetChangeOnlyProgrammaticaly(bool value) {m_onlyProgrammaticalChange = value;}
signals:
    void currentIndexChanged(int index);
    void changeRequested(int index);
    
public:
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    
};

#endif // SWITCHER_H
