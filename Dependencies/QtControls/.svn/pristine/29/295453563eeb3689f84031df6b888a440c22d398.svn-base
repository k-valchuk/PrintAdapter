#include "LiveWindowStream.h"

#ifdef _WIN32
    #include <windows.h>
#endif
#include "LiveWindow.h"

#include <QSharedPointer>

int LiveWindowStream::callbackLiveWindowFunc(const VideoStreamCallbackParams *pParams)
{
    if (!pParams)
        return 0;

    auto plw = static_cast<LiveWindowStream*>(pParams->pContext);
    if (plw) {
        emit plw->videoDataReceived(QSharedPointer<LWVideoData>::create(
                                    QByteArray((const char*)pParams->pBuffer, pParams->lBufferSize),
                                    pParams->lWidth, pParams->lHeight, pParams->lAspNum, pParams->lAspDen
                                    ));
    }
    
    return 0;
}

int LiveWindowStream::callbackPeakMeter(void *arg, int iArg, double *pMaxValue, double *pRMSValue, int nChannels)
{
    if (pMaxValue && pRMSValue && arg) {
            auto plw = static_cast<LiveWindowStream*>(arg);
            if (plw) {
    
                QMap<int, double> maxVals, rmsVals;
                for(int i = 0; i < LiveWinMaxAudioChannels; i++)
                {
                    maxVals[i] = pMaxValue[i];
                    rmsVals[i] = pRMSValue[i];
                }
    
                emit plw->audioDataReceived(QSharedPointer<LWAudioData>::create(
                                                std::move(maxVals),
                                                std::move(rmsVals),
                                                nChannels
                                                ));
            }
        }
    return 0;
}

LiveWindowStream::LiveWindowStream(QString url, QObject *parent)
    : IDataStream{parent}
{
    
    LiveWindowParams lwParams;
    lwParams.dwWidth = 0;
    lwParams.dwHeight = 0;

    CreateUrlLiveWindow((char*)url.toStdString().c_str(), &stream, LIVE_WINDOW_VER, &lwParams);

    stream->SetVideoStreamCallback(callbackLiveWindowFunc, this);
    stream->SetPeakMeterCallback(callbackPeakMeter, this, 0);
    //stream->SetSubtitlesStreamCallback(callbackSubtitles, this, 0);
}

LiveWindowStream::LiveWindowStream(const char *serverName, VSChannelID channelID, QObject *parent)
{
    CreateLiveWindow(serverName, channelID, &stream,  LIVE_WINDOW_VER);

    stream->SetVideoStreamCallback(callbackLiveWindowFunc, this);
    stream->SetPeakMeterCallback(callbackPeakMeter, this, 0);
    //stream->SetSubtitlesStreamCallback(callbackSubtitles, this, 0);
}

LiveWindowStream::~LiveWindowStream()
{
    DestroyLiveWindow(stream);
}

void LiveWindowStream::mute(bool muteSound)
{
    bool isMute = stream->IsMute();

    if(muteSound && !isMute)
        stream->Mute(true);

    if(!muteSound && isMute)
            stream->Mute(false);
}

bool LiveWindowStream::isMute()
{
    return stream->IsMute();
}
