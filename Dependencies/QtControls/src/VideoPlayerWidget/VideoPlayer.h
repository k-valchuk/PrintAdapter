#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QImage>
#include <QWidget>
#include "IDataStream.h"
#include "PeakMeterWidget.h"

class AnimatedTextLabel;

class VideoPlayer : public QWidget
{
    Q_OBJECT

    //Для установки цвета фона
    Q_PROPERTY(QColor playerBackground READ playerBackground WRITE setPlayerBackground);
    QColor playerBackground();
    void setPlayerBackground(QColor color);
    
    std::unique_ptr<QImage> m_frameImage;
    QSharedPointer<IDataStream> m_stream;
    QSize m_aspect{};
    
    int m_numChannels = 2;
    QVector<double> m_audioValues;
    PeakMeterWidget m_peakMeter;
    
    int m_maxVisibleAudioChannels = 8;
    int m_widthToMakePeakMeterWide = 820;
    bool m_drawPeakMeter = true;
    int m_peakMeterOpacity = 200;

    QColor m_backgroundColor{};
    bool m_needPaintBackgroundColor = false;

    //Прямоугольник отрисовки кадра
    QRect m_frameDrawRect{};

    // Плашака текста с анимацией opacity
    AnimatedTextLabel* m_animatedLbl = nullptr;
    // Нужна ли отрисовка анимированного текста
    bool m_paintText = false;
    // Смещение точки начала рисования анимированного текста по оси x относительно левой верхней точки отрисованного кадра
    int m_dxPoint = 50;
    // Смещение точки начала рисования анимированного текста по оси y относительно левой верхней точки отрисованного кадра
    int m_dyPoint = 30;
    // Значения при достижении которых точка рисования анимированного текста начинает смещаться ближе к левой верхней точке отрисовки кадра
    const int m_minimalNormalWidth = 400;
    const int m_minimalNormalHeight = 200;
    // Соотношения расположения фразы относительно кадра после достижения минимальных значений
    // В случае отсутствия кадра - соотношения расположения относительно прямоугольника плеера
    const int m_pointToHeightRatio = 8;
    const int m_pointToWidthRatio = 7;

public:
    explicit VideoPlayer(QWidget* parent = nullptr);
    explicit VideoPlayer(QSharedPointer<IDataStream> stream, QWidget* parent = nullptr);
    ~VideoPlayer();
    
    void SetStream(QSharedPointer<IDataStream> stream);
    QSharedPointer<IDataStream> GetStream();

    int maxVisibleAudioChannels() const;
    void setMaxVisibleAudioChannels(int newMaxVisibleAudioChannels);
    
    int widthToMakePeakMeterWide() const;
    void setWidthToMakePeakMeterWide(int newWidthToMakePeakMeterWide);
    
    bool drawPeakMeter() const;
    void setDrawPeakMeter(bool newDrawPeakMeter);
    
    int peakMeterOpacity() const;
    void setPeakMeterOpacity(int newPeakMeterOpacity);

    // Включает отрисовку кастомного цвета заднего фона если необходимо
    void paintBackgroundColor(bool needPaint);

    // Создать прозрачную плашку с текстом
    void createAnimatedLabel();
    // Вернуть прозрачную плашку с текстом
    AnimatedTextLabel* getAnimatedLabel();
    // Установить текст для анимированной плашки текста
    void setTextToPaint(QString text);
    // Включает/отключает отрисовку анимированной плашки текста
    void enableTextPaint(bool enable);
    // Установить цвет букв для анимированной плашки текста
    void setTextBrush(QColor color);
    // Установить шрифт для анимированной плашки текста
    void setTextFont(QFont font);
    // Установить смещение точки начала рисования анимированного текста по оси x относительно левой верхней точки отрисованного кадра
    void setTextPointdx(int dx);
    // Установить смещение точки начала рисования анимированного текста по оси y относительно левой верхней точки отрисованного кадра
    void setTextPointdy(int dy);

    //Получить позицию отрисовки кадра в плеере
    QRect getFrameDrawRect();

    // Вернет высоту всего виджета в зависимости от переданной ширины кадра
    int widgetHeightFromFrameWidth(int width);

private:
    // Определяет позицию отрисовки кадра
    QRect scaleFrameRect(QRect widgetGeometry);

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);
    
private slots:
    void onVideoDataReceived    (QSharedPointer<IVideoData> data);
    void onAudioDataReceived    (QSharedPointer<IAudioData> data);
    void onSubtitlesDataReceived(QSharedPointer<ISubtitlesData> data);

signals:
    void sig_frameRectChaged(QRect rect);
};

#endif // VIDEOPLAYER_H
