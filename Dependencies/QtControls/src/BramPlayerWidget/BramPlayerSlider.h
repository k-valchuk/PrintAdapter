//-----------------------------------------------------------------------------------------------------------------------
//Слайдер BramPlayer
// возможности:
//- отметить маркеры
//- выделить markIn, markOut
//- прыжок ползунка в позицию курсора
//-----------------------------------------------------------------------------------------------------------------------
#ifndef BRAMPLAYERSLIDER_H
#define BRAMPLAYERSLIDER_H

#include "QSlider"
class BramPlayerSlider : public QSlider
{
    Q_OBJECT
public:
    //маркер слайдера
    struct SliderMarker
    {
        ulong posStart;
        ulong posEnd;
    };

    BramPlayerSlider(QWidget *parent = nullptr);

    void setMarkers(const QVector<SliderMarker> &marVec);                 //задать маркеры
    void setMarkIn();                                            //установить markIn
    void setMarkOut();                                           //установить markOut
    void clearMarkInOut();                                       //очистить маркины
    void setMarkInMarkOut(int in, int out);                      //задать маркины
    void setEnabled(bool bEn);                                   //доступность слайдера
    void setValueByUser(int value);                              //установить значение слайдера с вызовом сигнала ChangedByUser
    void goNextMarker();                                         //перейти на следующий маркер
    void goPrevMarker();                                         //перейти на предыдущий маркер
    void goToMarkIn();                                           //перейти к markIn
    void goToMarkOut();                                          //перейти к markOut
protected:
    virtual void paintEvent(QPaintEvent *) override;
    virtual void mousePressEvent(QMouseEvent *me) override;
    virtual void mouseMoveEvent(QMouseEvent *me) override;

signals:
    void sig_valueChangedByUser(int value);     //положение слайдера изменено пользователем
    void sig_changedMarkInMarkOut(int, int);    //markIn и markOut изменены

private:
    QVector<int> markers;                       //маркеры
    int markIn = -1;                            //позиция markIn
    int markOut = -1;                           //позиция markOut
    bool bEnabled = true;                       //доступность

    void setSliderValueToMousePosition(QMouseEvent *me);        //установить позицию ползунка в позицию мыши
};

#endif // BRAMPLAYERSLIDER_H
