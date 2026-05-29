#ifndef LIVEWINDOWSTREAM_H
#define LIVEWINDOWSTREAM_H

#include "IDataStream.h"

#include <QMap>

#include "Inc/VInc/VChannel.h"

class ILiveWindow;
class VideoStreamCallbackParams;
class LiveWindowStream : public IDataStream
{   
    static int callbackLiveWindowFunc(const VideoStreamCallbackParams* pParams);
    static int callbackPeakMeter(void* arg, int iArg, double* pMaxValue, double* pRMSValue, int nChannels);
    
    class LWVideoData : public IVideoData
    {
        QByteArray data;
        QSize res;
        QSize aspect;
    public:
        LWVideoData(QByteArray&& data, int w, int h, int ax, int ay)
        {
            this->data = std::move(data);
            res = QSize(w, h);
            aspect = QSize(ax, ay);
        }
        QSize GetResolution() override { return res; }
        QSize GetAspectRatio()override { return aspect; }
        QByteArray& GetData() override { return data;}
    };
    
    class LWAudioData : public IAudioData
    {
        QMap<int, double> m_maxVals, m_rmsVals;
        int m_nChannels;
    public:
        LWAudioData(QMap<int, double>&& maxVals, QMap<int, double>&& rmsVals, int nChannels)
        {
            m_maxVals = std::move(maxVals);
            m_rmsVals = std::move(rmsVals);
            m_nChannels = nChannels;
        }
        double GetMaxValue(int channel) override
        {
            if(m_maxVals.contains(channel))
                return m_maxVals[channel];
            else
                return 0.0;
        }
        double GetRMSValue(int channel) override
        {
            if(m_rmsVals.contains(channel))
                return m_rmsVals[channel];
            else
                return 0.0;
        }
        int    GetNChannels()           override
        {
            return m_nChannels;
        }
    };

    
    ILiveWindow* stream;
public:
    explicit LiveWindowStream(QString url, QObject *parent = nullptr);
    explicit LiveWindowStream(const char *serverName, VSChannelID channelID, QObject *parent = nullptr);

    ~LiveWindowStream();

    void mute(bool muteSound);
    bool isMute();
};

#endif // LIVEWINDOWSTREAM_H
