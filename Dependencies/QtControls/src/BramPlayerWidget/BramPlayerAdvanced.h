//-----------------------------------------------------------------------------------------------------------------------
//Расширенный Bram Player
// - кнопки управления клипом
// - кастомный слайдер с маркерами и маркинами
//-----------------------------------------------------------------------------------------------------------------------
#ifndef BRAMPLAYERADVANCED_H
#define BRAMPLAYERADVANCED_H

#include <QFrame>
#include "TCEditWidget.h"
#include "BramPlayerSlider.h"

class BramPlayerHeirWidget;
class AspectRatioSingleItemLayout;
namespace Ui {
class BramPlayerAdvanced;
}

class BramPlayerAdvanced : public QFrame
{
    Q_OBJECT

    //режим размера плеера
    enum class SizeMode
    {
        fullSizeMode,       //полный
        smallSizeMode       //урезанный
    };

public:
    explicit BramPlayerAdvanced(QWidget *parent = 0);
    ~BramPlayerAdvanced();

    //добавить клип
    void appendClip(const QString &filePath, ulong fileDuration, double aspRatio, ulong duration = 0, ulong markIn = 0);
    void clear();                                                                                       //очистить плеер
    void setMarkers(const QVector<BramPlayerSlider::SliderMarker> &markers);                            //установить маркеры
    TC getPlayerPosition() const;                                                                       //получить позицию плеера
    void setPlayerPosition(TC pos);                                                                     //установить позицию плеера

protected:
    virtual void resizeEvent(QResizeEvent *event);      //события при изменении размера

private slots:
    void on_play_pushButton_clicked();                  // play\stop
    void on_frameForward_pushButton_clicked();          //кадр вперед
    void on_frameBack_pushButton_clicked();             //кадр назад
    void on_seekBegin_pushButton_clicked();             //перейти на маркер в начало
    void on_seekEnd_pushButton_clicked();               //перейти на маркер в конец
    void on_markOut_pushButton_clicked();               //markOut
    void on_markIn_pushButton_clicked();                //markIn
    void on_goToOut_pushButton_clicked();               //перейти к markOut
    void on_goToIn_pushButton_clicked();                //перейти к markIn

    void slot_playerPosChanged(int pos);                //изменена позиция плеера
    void slot_sliderBramValueChanged(int value);        //положение слайдера изменилось пользователем
    void slot_TCEdited(TC value);                       //изменен TC
    void slot_changedMarkInMarkOut(int in, int out);    //изменены markIn и markOut
    void slot_tcInEdited(TC value);                     //изменен TC markIn
    void slot_tcOutEdited(TC value);                    //изменен TC markOut

    void slot_stopPlayer();                             //остановить плеер

private:
    Ui::BramPlayerAdvanced *ui;
    BramPlayerHeirWidget *bramPlayer;                   //плеер
    AspectRatioSingleItemLayout *aspRatLay;             //компоновка c aspectRatio

    SizeMode sizeMode = SizeMode::fullSizeMode;         //полный размер по умолчанию
    int minWidthFullSize = 0;                           //минимальная ширина виджета в режиме полного размера
    bool bIsPlaying = false;                              //идентификатор play

    void setSizeMode(SizeMode sm);                      //установить режим размера
    void setPlayerEnabled(bool bEn);                    //доступность элементов плеера
};

#endif // BRAMPLAYERADVANCED_H
