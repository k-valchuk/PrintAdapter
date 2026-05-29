#include "BramPlayerAdvanced.h"
#include "ui_BramPlayerAdvanced.h"
#include "BramPlayerHeirWidget.h"
#include "AspectRatioSingleItemLayout.h"

BramPlayerAdvanced::BramPlayerAdvanced(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BramPlayerAdvanced)
{
    ui->setupUi(this);

    bramPlayer = new BramPlayerHeirWidget();
    aspRatLay = new AspectRatioSingleItemLayout();
    aspRatLay->addWidget(bramPlayer);
    ui->main_verticalLayout->insertLayout(0, aspRatLay);

    bramPlayer->setPlayerSize(BramPlayer::EPleerSize::ePSFHD);

    minWidthFullSize = ui->horizontalLayout_mark->sizeHint().width();//мин. размер смены режима отображения плеера

    setPlayerEnabled(false);//не доступен по умолчанию
    //изменена позиция плеера
    connect(bramPlayer, SIGNAL(posChanged(int)), SLOT(slot_playerPosChanged(int)));
    //значение слайдера изменено пользователем
    connect(ui->sliderBram, SIGNAL(sig_valueChangedByUser(int)), this, SLOT(slot_sliderBramValueChanged(int)));
    //изменен основной таймкод
    connect(ui->TC_lineEdit, SIGNAL(tcEdited(TC)), this, SLOT(slot_TCEdited(TC)));
    //изменен markIn и markOut в слайдере
    connect(ui->sliderBram, SIGNAL(sig_changedMarkInMarkOut(int,int)), this, SLOT(slot_changedMarkInMarkOut(int,int)));
    //изменен markIn в TC
    connect(ui->TCIn_lineEdit, SIGNAL(tcEdited(TC)), this, SLOT(slot_tcInEdited(TC)));
    //изменен markOut в TC
    connect(ui->TCOut_lineEdit, SIGNAL(tcEdited(TC)), this, SLOT(slot_tcOutEdited(TC)));
    //остановить плеер при выборе пункта контекстного меню
    connect(bramPlayer, SIGNAL(sig_contexMenuActChecked()), this, SLOT(slot_stopPlayer()));
}

BramPlayerAdvanced::~BramPlayerAdvanced()
{
    delete ui;
}

void BramPlayerAdvanced::appendClip(const QString &filePath, ulong fileDuration, double aspRatio, ulong duration, ulong markIn)
{
    if (!filePath.isEmpty())
    {
        bramPlayer->clear();
        bramPlayer->appendClip(filePath);

        setPlayerEnabled(true);
        aspRatLay->setAspectRatio(aspRatio);
        ui->sliderBram->setMaximum(fileDuration);
        ui->sliderBram->setValue(0);
        ui->TCDuration_lineEdit->setTC(duration);
        ui->TCOut_lineEdit->setTC(duration);
        ui->sliderBram->setMarkInMarkOut(markIn, markIn + duration);
    }
    else
    {
        clear();
    }
}

void BramPlayerAdvanced::clear()
{
    if (bIsPlaying)
        on_play_pushButton_clicked();
    bramPlayer->clear();
    bramPlayer->reload();
    ui->sliderBram->setValue(0);
    ui->TC_lineEdit->setTC(0);
    ui->TCOut_lineEdit->setTC(0);
    ui->TCIn_lineEdit->setTC(0);
    ui->TCDuration_lineEdit->setTC(0);
    setPlayerEnabled(false);

}

void BramPlayerAdvanced::setMarkers(const QVector<BramPlayerSlider::SliderMarker> &markers)
{
    ui->sliderBram->setMarkers(markers);
}

TC BramPlayerAdvanced::getPlayerPosition() const
{
    return bramPlayer->pos();
}

void BramPlayerAdvanced::setPlayerPosition(TC pos)
{
    ui->sliderBram->setValueByUser(pos);
}

void BramPlayerAdvanced::resizeEvent(QResizeEvent *event)
{
    //проверка на переход в другой режим размера
    if (sizeMode == SizeMode::smallSizeMode)
    {
        if (event->size().width() >= minWidthFullSize)
            setSizeMode(SizeMode::fullSizeMode);
    }
    else
    {
        if (event->size().width() < minWidthFullSize)
            setSizeMode(SizeMode::smallSizeMode);
    }
}

void BramPlayerAdvanced::on_play_pushButton_clicked()
{
    if (bIsPlaying)
    { //останавливаем плеер
        ui->play_pushButton->setIcon(QIcon(":/playerIcons/plPlay"));
        ui->play_pushButton->setToolTip(tr("Play"));
        bIsPlaying = false;
        bramPlayer->stop();
    }
    else //запускаем плеер
    {
        ui->play_pushButton->setIcon(QIcon(":/playerIcons/plStop"));
        ui->play_pushButton->setToolTip(tr("Stop"));
        bIsPlaying = true;
        bramPlayer->play();
    }
}

void BramPlayerAdvanced::slot_playerPosChanged(int pos)
{
    if (bramPlayer->state() == BramPlayer::EPlayerState::ePSPlaying && bIsPlaying)
    {
        //если клип проигран
        if (ui->sliderBram->maximum() < pos)
        {
            on_play_pushButton_clicked();
            pos = ui->sliderBram->maximum();
        }

        ui->sliderBram->setValue(pos);
        ui->TC_lineEdit->setTC(pos);
    }
}

void BramPlayerAdvanced::slot_sliderBramValueChanged(int value)
{
    if (bIsPlaying)//останавливаем плеер
        on_play_pushButton_clicked();

    ui->TC_lineEdit->setTC(value);
    bramPlayer->seek(value);
}

void BramPlayerAdvanced::slot_TCEdited(TC value)
{
    if (value > (uint)ui->sliderBram->maximum())
    {//выход за границы максимума
        value = ui->sliderBram->maximum();
        ui->TC_lineEdit->setTC(value);
    }

    if (bIsPlaying)//останавливаем плеер
        on_play_pushButton_clicked();
    ui->sliderBram->setValue(value);
    bramPlayer->seek((uint)value);
}

void BramPlayerAdvanced::slot_changedMarkInMarkOut(int in, int out)
{
    ui->TCIn_lineEdit->setTC(in);
    ui->TCOut_lineEdit->setTC(out);
}

void BramPlayerAdvanced::slot_tcInEdited(TC value)
{
    if (value >= (uint)ui->sliderBram->maximum())
    {//In и Out в максимум
        value = ui->sliderBram->maximum();
        ui->TCOut_lineEdit->setTC(value);
        ui->TCIn_lineEdit->setTC(value);
    }
    else if (value > ui->TCOut_lineEdit->getCurrentTC())
    {//если In больше Out
        ui->TCOut_lineEdit->setTC(value + 1);
    }
    //обновить
    ui->sliderBram->setMarkInMarkOut(value, ui->TCOut_lineEdit->getCurrentTC());
    ui->sliderBram->setValueByUser(value);
}

void BramPlayerAdvanced::slot_tcOutEdited(TC value)
{
    if (value > (uint)ui->sliderBram->maximum())
    {//выход за максимум
        value = ui->sliderBram->maximum();
        ui->TCOut_lineEdit->setTC(value);
    }
    else if (value <= ui->TCIn_lineEdit->getCurrentTC())
    {//еслиOut меньше In
        if (value != 0)
            ui->TCIn_lineEdit->setTC(value - 1);
        else
            ui->TCIn_lineEdit->setTC(0);
    }
    //обновить
    ui->sliderBram->setMarkInMarkOut(ui->TCIn_lineEdit->getCurrentTC(), value);
    ui->sliderBram->setValueByUser(value);
}

void BramPlayerAdvanced::slot_stopPlayer()
{
    if (bIsPlaying)//останавливаем плеер
        on_play_pushButton_clicked();
}

void BramPlayerAdvanced::setSizeMode(BramPlayerAdvanced::SizeMode sm)
{
    if (sm == sizeMode)
        return;
    bool bVis = true;
    //изменяем компоновку
    if (sm == SizeMode::smallSizeMode)//урезанный
    {
        bVis = false;
        QLayoutItem *it = ui->horizontalLayout_TC->takeAt(4);
        ui->main_verticalLayout->insertItem(3, it);
        ui->horizontalSpacerPlay_In->changeSize(0 ,0 , QSizePolicy::Expanding);
        ui->horizontalSpacerPlay_Out->changeSize(0 ,0 , QSizePolicy::Expanding);
    }
    else //полный
    {
        QLayoutItem *it = ui->main_verticalLayout->takeAt(3);
        ui->horizontalLayout_TC->insertItem(4, it);
        ui->horizontalSpacerPlay_In->changeSize(0 ,0 , QSizePolicy::Fixed);
        ui->horizontalSpacerPlay_Out->changeSize(0 ,0 , QSizePolicy::Fixed);
    }

    ui->TCDuration_lineEdit->setVisible(bVis);
    ui->dur_label->setVisible(bVis);
    ui->in_label->setVisible(bVis);
    ui->out_label->setVisible(bVis);

    sizeMode = sm;
}

void BramPlayerAdvanced::setPlayerEnabled(bool bEn)
{
    ui->sliderBram->setEnabled(bEn);

    ui->markIn_pushButton->setEnabled(bEn);
    ui->markOut_pushButton->setEnabled(bEn);
    ui->TC_lineEdit->setReadOnly(!bEn);

    ui->seekBegin_pushButton->setEnabled(bEn);
    ui->seekEnd_pushButton->setEnabled(bEn);
    ui->frameBack_pushButton->setEnabled(bEn);
    ui->frameForward_pushButton->setEnabled(bEn);
    ui->play_pushButton->setEnabled(bEn);
    //ui->playBlock_pushButton->setEnabled(bEn);

    ui->goToIn_pushButton->setEnabled(bEn);
    ui->goToOut_pushButton->setEnabled(bEn);
    ui->TCIn_lineEdit->setReadOnly(!bEn);
    ui->TCOut_lineEdit->setReadOnly(!bEn);
}


void BramPlayerAdvanced::on_frameForward_pushButton_clicked()
{
    ui->sliderBram->setValueByUser(ui->sliderBram->value() + 1);
}

void BramPlayerAdvanced::on_frameBack_pushButton_clicked()
{
    ui->sliderBram->setValueByUser(ui->sliderBram->value() - 1);
}

void BramPlayerAdvanced::on_seekBegin_pushButton_clicked()
{
    ui->sliderBram->goPrevMarker();
}

void BramPlayerAdvanced::on_seekEnd_pushButton_clicked()
{
    ui->sliderBram->goNextMarker();
}

void BramPlayerAdvanced::on_markOut_pushButton_clicked()
{
    ui->sliderBram->setMarkOut();
}

void BramPlayerAdvanced::on_markIn_pushButton_clicked()
{
    ui->sliderBram->setMarkIn();
}

void BramPlayerAdvanced::on_goToOut_pushButton_clicked()
{
    ui->sliderBram->goToMarkOut();
}

void BramPlayerAdvanced::on_goToIn_pushButton_clicked()
{
    ui->sliderBram->goToMarkIn();
}
