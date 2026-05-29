#include "BramPlayerHeirWidget.h"
#include "qmenu.h"

BramPlayerHeirWidget::BramPlayerHeirWidget(QWidget *parent, Qt::WindowFlags f) : BramPlayerWidget(parent, f)
{
    initMenu();
}

void BramPlayerHeirWidget::initMenu()
{
    contexMenu = new QMenu();
    //обновить
    contexMenu->addAction(tr("Update"), this, SLOT(slot_updatePlayer()));
    contexMenu->addSeparator();

    //группа пикметр
    QActionGroup *actGr = new QActionGroup(this);
    actGr->setExclusive(true);
    //не показывать
    QAction *act = contexMenu->addAction(tr("Hide peakmeters"));
    act->setCheckable(true);
    act->setChecked(true);
    act->setProperty("chCount", 0);
    actGr->addAction(act);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_setPeakMeterCount()));
    //1 канал
    act = contexMenu->addAction(tr("1 channel peakmeter"));
    act->setCheckable(true);
    act->setProperty("chCount", 1);
    actGr->addAction(act);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_setPeakMeterCount()));
    //2 канала
    act = contexMenu->addAction(tr("2 channels peakmeter"));
    act->setCheckable(true);
    act->setProperty("chCount", 2);
    actGr->addAction(act);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_setPeakMeterCount()));
    //4 канала
    act = contexMenu->addAction(tr("4 channels peakmeter"));
    act->setCheckable(true);
    act->setProperty("chCount", 4);
    actGr->addAction(act);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_setPeakMeterCount()));
    contexMenu->addSeparator();

    //группа разрешение плеера
    actGr = new QActionGroup(this);
    actGr->setExclusive(true);
    //"360x288"
    act = contexMenu->addAction("360x288");
    act->setCheckable(true);
    act->setProperty("res", (int)BramPlayer::EPleerSize::ePSLowRes);
    actGr->addAction(act);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_setResolution()));
    //"720x576"
    act = contexMenu->addAction("720x576");
    act->setCheckable(true);
    act->setProperty("res", (int)BramPlayer::EPleerSize::ePSSD);
    actGr->addAction(act);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_setResolution()));
    // "1920x1080"
    act = contexMenu->addAction( "1920x1080");
    act->setCheckable(true);
    act->setChecked(true);
    act->setProperty("res", (int)BramPlayer::EPleerSize::ePSFHD);
    actGr->addAction(act);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_setResolution()));
    contexMenu->addSeparator();

    //звук при перемотке
    act = contexMenu->addAction(tr("Audio scrubbing"));
    act->setCheckable(true);
    act->setChecked(true);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_audioScrub(bool)));

    //TODO:
//    //отображать графику
//    act = contexMenu->addAction(tr("Show graphics"));
//    act->setCheckable(true);
//    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_showGraphics(bool)));
//    //отображать субтитры
//    act = contexMenu->addAction(tr("Show subtitles"));
//    act->setCheckable(true);
//    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_showSubtitles(bool)));

    //деинтерлейс
    act = contexMenu->addAction(tr("Deinterlace"));
    act->setCheckable(true);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(slot_deinterlace(bool)));
}

void BramPlayerHeirWidget::slot_updatePlayer()
{
    emit sig_contexMenuActChecked();
    reload();
}

void BramPlayerHeirWidget::slot_setPeakMeterCount()
{
    emit sig_contexMenuActChecked();
    int count = QObject::sender()->property("chCount").toInt();
    setPeekmeterCount(count);
    reload();
}

void BramPlayerHeirWidget::slot_setResolution()
{
    emit sig_contexMenuActChecked();
    setPlayerSize((BramPlayer::EPleerSize)sender()->property("res").toInt());
    reload();
}

void BramPlayerHeirWidget::slot_audioScrub(bool bS)
{
    emit sig_contexMenuActChecked();
    setAudioScrub(bS);
    reload();
}

void BramPlayerHeirWidget::slot_showGraphics(bool bS)
{

}

void BramPlayerHeirWidget::slot_showSubtitles(bool bS)
{

}

void BramPlayerHeirWidget::slot_deinterlace(bool bS)
{
    emit sig_contexMenuActChecked();
    setDeinterlace(bS);
    reload();
}
