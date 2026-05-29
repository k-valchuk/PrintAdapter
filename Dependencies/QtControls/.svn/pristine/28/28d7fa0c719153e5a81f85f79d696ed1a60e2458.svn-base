#ifndef QUICKFILTERTABBUTTONS_H
#define QUICKFILTERTABBUTTONS_H

#include <QFrame>

namespace Ui {
class QuickFilterTabButtons;
}

class QuickFilterTabButtons : public QFrame
{
    Q_OBJECT
    
public:
    explicit QuickFilterTabButtons(QWidget *parent = nullptr);
    ~QuickFilterTabButtons();
    
    void SetVisibility(bool p1, bool p2, bool p3);
    
    void SetSeletedFavoutitesCount(int count);
    void SetSelected(int page);
private:
    Ui::QuickFilterTabButtons *ui;
      
signals:
    void sig_page1();
    void sig_page2();
    void sig_page3();
    
public slots:
    void page1() {emit sig_page1();}
    void page2() {emit sig_page2();}
    void page3() {emit sig_page3();}
};

#endif // QUICKFILTERTABBUTTONS_H
