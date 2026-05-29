#include "QuickFilterTabButtons.h"
#include "qstyle.h"
#include "ui_QuickFilterTabButtons.h"
#include <QColor>

QuickFilterTabButtons::QuickFilterTabButtons(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QuickFilterTabButtons)
{
    ui->setupUi(this);
    ui->btn1->SetText("");
    ui->btn2->SetText("");
    ui->btn3->SetText("");
}

QuickFilterTabButtons::~QuickFilterTabButtons()
{
    delete ui;
}

void QuickFilterTabButtons::SetVisibility(bool p1, bool p2, bool p3)
{
    ui->btn1->setVisible(p1);
    ui->btn2->setVisible(p2);
    ui->btn3->setVisible(p3);
}

void QuickFilterTabButtons::SetSeletedFavoutitesCount(int count)
{
    ui->btn1->SetText(count != 0 ? QString("%1").arg(count) : "");
    if(count > 0)
        ui->btn1->SetTextColor(QColor("#6288BF"));
    else
        ui->btn1->SetTextColor(QApplication::palette().color(QPalette::ColorRole::Text));
}

void QuickFilterTabButtons::SetSelected(int page)
{
    QColor col1 = page == 0 ? QColor("#6288BF") : QApplication::palette().color(QPalette::ColorRole::Text);
    QColor col2 = page == 1 ? QColor("#6288BF") : QApplication::palette().color(QPalette::ColorRole::Text);
    QColor col3 = page == 2 ? QColor("#6288BF") : QApplication::palette().color(QPalette::ColorRole::Text);
    
    ui->btn1->SetColor(col1);
    ui->btn2->SetColor(col2);
    ui->btn3->SetColor(col3);
    
    QIcon icon1 = page == 0 ? QIcon(":/filter/star_blue") : QIcon(":/filter/star_gray");
    QIcon icon2 = page == 1 ? QIcon(":/filter/presets_blue") : QIcon(":/filter/presets_gray");
    QIcon icon3 = page == 2 ? QIcon(":/filter/history_blue") : QIcon(":/filter/history_gray");
    
    ui->btn1->SetIcon(icon1);
    ui->btn2->SetIcon(icon2);
    ui->btn3->SetIcon(icon3);
}
