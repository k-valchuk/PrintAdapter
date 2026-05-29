#ifndef LOADINGOVERLAY_H
#define LOADINGOVERLAY_H

#include <QFrame>
#include <QMovie>

namespace Ui {
class LoadingOverlay;
}

class LoadingOverlay : public QFrame
{
    Q_OBJECT
    QMovie m_loadingGif;
public:
    explicit LoadingOverlay(QWidget *parent = nullptr);
    ~LoadingOverlay();
    
private:
    Ui::LoadingOverlay *ui;
};

#endif // LOADINGOVERLAY_H
