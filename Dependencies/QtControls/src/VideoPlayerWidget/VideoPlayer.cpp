#include "VideoPlayer.h"

#include <QPainter>
#include <cmath>
#include <QPaintEvent>
#include "AnimatedTextLabel.h"

//----------------------------------------------------------------------------
QColor VideoPlayer::playerBackground(){
    return m_backgroundColor;
}

//----------------------------------
void VideoPlayer::setPlayerBackground(QColor color){
    m_backgroundColor = color;
}

//----------------------------------------------------------------------------
int VideoPlayer::maxVisibleAudioChannels() const
{
    return m_maxVisibleAudioChannels;
}

//--------------------------------------
void VideoPlayer::setMaxVisibleAudioChannels(int newMaxVisibleAudioChannels)
{
    m_maxVisibleAudioChannels = newMaxVisibleAudioChannels;
    this->update();
}

//----------------------------------------------------------------------------
int VideoPlayer::widthToMakePeakMeterWide() const
{
    return m_widthToMakePeakMeterWide;
}

//-------------------------------------
void VideoPlayer::setWidthToMakePeakMeterWide(int newWidthToMakePeakMeterWide)
{
    if(newWidthToMakePeakMeterWide < 0)
        newWidthToMakePeakMeterWide = 0;
    m_widthToMakePeakMeterWide = newWidthToMakePeakMeterWide;
    this->update();
}

//-------------------------------------
bool VideoPlayer::drawPeakMeter() const
{
    return m_drawPeakMeter;
}

//-------------------------------------
void VideoPlayer::setDrawPeakMeter(bool newDrawPeakMeter)
{
    m_drawPeakMeter = newDrawPeakMeter;
    this->update();
}

//--------------------------------------
int VideoPlayer::peakMeterOpacity() const
{
    return m_peakMeterOpacity;
}

//---------------------------------------
void VideoPlayer::setPeakMeterOpacity(int newPeakMeterOpacity)
{
    if(newPeakMeterOpacity < 0)
        newPeakMeterOpacity = 0;
    if(newPeakMeterOpacity > 255)
        newPeakMeterOpacity = 255;
    
    m_peakMeterOpacity = newPeakMeterOpacity;
    this->update();
}

//----------------------------------------------------------------------------
void VideoPlayer::paintBackgroundColor(bool needPaint)
{
    m_needPaintBackgroundColor = needPaint;
}

//----------------------------------------------------------------------------
// Прозрачная плашка анимации текста
//----------------------------------------------
void VideoPlayer::createAnimatedLabel()
{
    if(!m_animatedLbl)
        m_animatedLbl = new AnimatedTextLabel(this);
}

//----------------------------------------------
AnimatedTextLabel* VideoPlayer::getAnimatedLabel()
{
    return m_animatedLbl;
}

//----------------------------------------------
void VideoPlayer::setTextToPaint(QString text)
{
    if(m_animatedLbl)
        m_animatedLbl->setLabelText(text);
}

//-----------------------------------------------
void VideoPlayer::enableTextPaint(bool enable)
{
    m_paintText = enable;
    if(m_animatedLbl)
        m_animatedLbl->setTextVisible(enable);
}

//----------------------------------------------
void VideoPlayer::setTextBrush(QColor color)
{
    if(m_animatedLbl)
        m_animatedLbl->setTextBrush(color);
}

//----------------------------------------------
void VideoPlayer::setTextFont(QFont font)
{
    if(m_animatedLbl)
        m_animatedLbl->setTextFont(font);
}

//----------------------------------------------
void VideoPlayer::setTextPointdx(int dx)
{
    m_dxPoint = dx;
}

//----------------------------------------------
void VideoPlayer::setTextPointdy(int dy)
{
    m_dyPoint = dy;
}

//----------------------------------------------------------------------------
VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget{parent}
{
    m_stream = nullptr;
}

//-----------------------------------------
VideoPlayer::VideoPlayer(QSharedPointer<IDataStream> stream, QWidget *parent)
    : QWidget{parent}
{
    m_stream = stream;
    connect(stream.data(), &IDataStream::videoDataReceived, this, &VideoPlayer::onVideoDataReceived, 
            Qt::QueuedConnection);
    connect(stream.data(), &IDataStream::audioDataReceived, this, &VideoPlayer::onAudioDataReceived, 
            Qt::QueuedConnection);
    connect(stream.data(), &IDataStream::subtitlesDataReceived, this, &VideoPlayer::onSubtitlesDataReceived, 
            Qt::QueuedConnection);
}

//-----------------------------------------
VideoPlayer::~VideoPlayer()
{
    if(m_stream)
    {
        disconnect(m_stream.data(), &IDataStream::videoDataReceived, this, &VideoPlayer::onVideoDataReceived);
        disconnect(m_stream.data(), &IDataStream::audioDataReceived, this, &VideoPlayer::onAudioDataReceived);
        disconnect(m_stream.data(), &IDataStream::subtitlesDataReceived, this, &VideoPlayer::onSubtitlesDataReceived);
    }
}

//----------------------------------------------------------------------------
void VideoPlayer::SetStream(QSharedPointer<IDataStream> stream)
{
    if(m_stream)
    {
        disconnect(m_stream.data(), &IDataStream::videoDataReceived, this, &VideoPlayer::onVideoDataReceived);
        disconnect(m_stream.data(), &IDataStream::audioDataReceived, this, &VideoPlayer::onAudioDataReceived);
        disconnect(m_stream.data(), &IDataStream::subtitlesDataReceived, this, &VideoPlayer::onSubtitlesDataReceived);
    }
    
    m_stream = stream;
    connect(stream.data(), &IDataStream::videoDataReceived, this, &VideoPlayer::onVideoDataReceived, 
            Qt::QueuedConnection);
    connect(stream.data(), &IDataStream::audioDataReceived, this, &VideoPlayer::onAudioDataReceived, 
            Qt::QueuedConnection);
    connect(stream.data(), &IDataStream::subtitlesDataReceived, this, &VideoPlayer::onSubtitlesDataReceived, 
            Qt::QueuedConnection);
}

//-------------------------------------
QSharedPointer<IDataStream> VideoPlayer::GetStream()
{
    return m_stream;
}

//----------------------------------------------------------------------------
void VideoPlayer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // Пересчитываем позицию отрисовки кадра
    scaleFrameRect(this->geometry());
}

//-------------------------------------
void VideoPlayer::paintEvent(QPaintEvent *event)
{
    if(!m_frameImage)
    {
        if(m_paintText && m_animatedLbl)
        {
            // При отсутствии кадра отрисовываем плашку в указанных соотношениях
            QRect frameRect = this->geometry();
            QPoint deltaPoint{};
            deltaPoint.setY((int)(double(frameRect.height()/m_pointToHeightRatio)));
            deltaPoint.setX((int)(double(frameRect.width()/m_pointToWidthRatio)));
            QPoint phrasePoint = deltaPoint;
            m_animatedLbl->move(phrasePoint);
        }
        return QWidget::paintEvent(event);
    }
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

    if(m_needPaintBackgroundColor)
        painter.fillRect(event->rect() , m_backgroundColor);

    // Позиция отрисовки кадра пересчитывается при изменении AspectRatio или при изменении геометрии виджета
    QRect rect = m_frameDrawRect;

    // Рисуем кадр
    QPixmap pixmap;
    pixmap.convertFromImage(*m_frameImage.get());
    painter.drawPixmap(rect, pixmap);

    // Отрисовываем плашку текста
    if(m_paintText && m_animatedLbl)
    {
        QRect frameRect = m_frameDrawRect;
        bool isNormalHeight = frameRect.height() > m_minimalNormalHeight? true : false;
        bool isNormalWidth = frameRect.width() > m_minimalNormalWidth? true : false;
        QPoint deltaPoint{m_dxPoint, m_dyPoint};
        if(!isNormalHeight)
            deltaPoint.setY((int)(double(frameRect.height()/m_pointToHeightRatio)));
        if(!isNormalWidth)
            deltaPoint.setX((int)(double(frameRect.width()/m_pointToWidthRatio)));
        QPoint phrasePoint = frameRect.topLeft() + deltaPoint - QPoint(0, 10);
        m_animatedLbl->move(phrasePoint);
    }
    
    if(m_drawPeakMeter)
    {
        PeakMeterWidget::EViewMode peakMode; 
        if(rect.width() < std::clamp(m_widthToMakePeakMeterWide, 0, 10'000))
            peakMode = PeakMeterWidget::EViewMode::eThinMode;
        else
            peakMode = PeakMeterWidget::EViewMode::eWideMode;
    
        int nChannels = std::clamp(m_numChannels, 0, m_maxVisibleAudioChannels);
        auto peakWidth = m_peakMeter.getWidgetWidth(peakMode, nChannels);
        auto peakStart = rect.right() - peakWidth - 20;
        rect.setLeft(peakStart);
        auto height = rect.height();
        rect.setTop(rect.top() + (int)height*0.08);
        rect.setBottom(rect.bottom() - (int)height*0.08);
    
        m_peakMeter.drawLegendBack(painter, nChannels, rect, peakMode, true, std::clamp(m_peakMeterOpacity - 20, 0, 255));
        m_peakMeter.drawLegendLines(painter, nChannels, rect, peakMode, true, std::clamp(m_peakMeterOpacity, 0, 255));
        m_peakMeter.drawPeaks(painter, m_audioValues, nChannels, rect, peakMode, std::clamp(m_peakMeterOpacity + 20, 0, 255));
    }
}

//----------------------------------------------------------------------------
void VideoPlayer::onVideoDataReceived(QSharedPointer<IVideoData> data)
{
    if ((m_frameImage && m_frameImage->size() != data->GetResolution()) || !m_frameImage)
        m_frameImage = std::make_unique<QImage>(data->GetResolution(), QImage::Format_RGB32);
    
    auto& bytes = data->GetData();
    
    for (int y = 0; y < m_frameImage->height(); y++)
        memcpy(m_frameImage->scanLine(y), (unsigned char*)bytes.data() + y * m_frameImage->bytesPerLine(), m_frameImage->bytesPerLine());

    if(m_aspect.height() != data->GetAspectRatio().height() && data->GetAspectRatio().height() > 0 ||
        m_aspect.width() != data->GetAspectRatio().width() && data->GetAspectRatio().width() > 0)
    {
        m_aspect = data->GetAspectRatio();
        // Пересчитываем позицию отрисовки кадра
        scaleFrameRect(this->geometry());
    }
    
    this->update();
}

//--------------------------------------
void VideoPlayer::onAudioDataReceived(QSharedPointer<IAudioData> data)
{
    m_audioValues.clear();
    m_numChannels = data->GetNChannels();

    for(int i = 0; i < data->GetNChannels(); i++)
        m_audioValues.push_back(data->GetMaxValue(i));
    
    this->update();
}

//----------------------------------------
void VideoPlayer::onSubtitlesDataReceived(QSharedPointer<ISubtitlesData> data)
{
    
}

//----------------------------------------------------------------------------
QRect VideoPlayer::getFrameDrawRect()
{
    return m_frameDrawRect;
}

//----------------------------------------------------------------------------
int VideoPlayer::widgetHeightFromFrameWidth(int width)
{
    return (int)width/(m_aspect.width()/(double)m_aspect.height());
}

//------------------------------------------
QRect VideoPlayer::scaleFrameRect(QRect widgetGeometry)
{
    QRect rect = widgetGeometry;

    if(widgetGeometry.width()/(double)widgetGeometry.height() > m_aspect.width()/(double)m_aspect.height())
    {
        int nwidth = (int)round(widgetGeometry.height() * (m_aspect.width()/(double)m_aspect.height()));
        rect.setLeft(rect.left() + widgetGeometry.width()/2 - nwidth/2);
        rect.setWidth(nwidth);
    }
    else if (widgetGeometry.width()/(double)widgetGeometry.height() < m_aspect.width()/(double)m_aspect.height())
    {
        int nheight = (int)round(widgetGeometry.width() * (m_aspect.height()/(double)m_aspect.width()));
        rect.setTop(rect.top() + widgetGeometry.height()/2 - nheight/2);
        rect.setHeight(nheight);
    }

    // Если позиция отрисовки кадра или размер кадра изменились - запоминаем и отправляем сигнал
    if(m_frameDrawRect.topLeft() != rect.topLeft() || m_frameDrawRect.height() != rect.height() || m_frameDrawRect.width() != rect.width())
    {
        m_frameDrawRect = rect;
        emit sig_frameRectChaged(m_frameDrawRect);
    }

    return rect;
}

