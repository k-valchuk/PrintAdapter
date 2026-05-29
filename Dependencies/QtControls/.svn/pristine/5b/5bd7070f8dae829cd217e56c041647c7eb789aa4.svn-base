//-----------------------------------------------------------------------------------------------------------------------
//Наследник BramPlayerWidget
// - перемотка по MouseMove
// - контекстное меню
//-----------------------------------------------------------------------------------------------------------------------
#ifndef BRAMPLAYERHEIRWIDGET_H
#define BRAMPLAYERHEIRWIDGET_H

#include "BramPlayerWidget.h"

class BramPlayerHeirWidget : public BramPlayerWidget
{
    Q_OBJECT
public:
    BramPlayerHeirWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);

protected:
    virtual void initMenu() override;                                   //создать меню

private slots:
    //контекстное меню плеера
    void slot_updatePlayer();                                           //обновить плеер
    void slot_setPeakMeterCount();                                      //задать количество каналов пикметра
    void slot_setResolution();                                          //задать разрешение
    void slot_audioScrub(bool bS);                                      //звук при перемотке
    void slot_showGraphics(bool bS);                                    //отображать графику
    void slot_showSubtitles(bool bS);                                   //отображать субтитры
    void slot_deinterlace(bool bS);                                     //деинтерлейс

signals:
    void sig_contexMenuActChecked();                                    //выбран пункт контекстного меню
};

#endif // BRAMPLAYERHEIRWIDGET_H
