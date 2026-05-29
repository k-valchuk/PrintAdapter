#include "LoadingOverlay.h"
#include "ui_LoadingOverlay.h"

LoadingOverlay::LoadingOverlay(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LoadingOverlay),
    m_loadingGif(":/LoadingOverlay/reloadGif")
{
    ui->setupUi(this);
    ui->gifLabel->setMovie(&m_loadingGif);
    m_loadingGif.start();
}

LoadingOverlay::~LoadingOverlay()
{
    delete ui;
}
