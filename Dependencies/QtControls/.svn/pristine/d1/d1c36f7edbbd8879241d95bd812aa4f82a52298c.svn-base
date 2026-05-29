#ifndef IDATASTREAM_H
#define IDATASTREAM_H

#include <memory>
#include <QObject>
#include <QSize>
#include <QSharedPointer>

class IVideoData
{
public:
    virtual QSize GetResolution() = 0;
    virtual QSize GetAspectRatio()= 0;
    virtual QByteArray& GetData() = 0;
    virtual ~IVideoData() {}
};

class IAudioData
{
public:
    virtual double GetMaxValue(int channel) = 0;
    virtual double GetRMSValue(int channel) = 0;
    virtual int    GetNChannels()           = 0;
    virtual ~IAudioData() {}
};

class ISubtitlesData
{
public:
    virtual QString GetSubtitles() = 0;
    virtual ~ISubtitlesData() {}
};

class IDataStream : public QObject
{
    Q_OBJECT
public:
    explicit IDataStream(QObject *parent = nullptr);
    virtual ~IDataStream() {}
    
signals:
    void videoDataReceived    (QSharedPointer<IVideoData> data);
    void audioDataReceived    (QSharedPointer<IAudioData> data);
    void subtitlesDataReceived(QSharedPointer<ISubtitlesData> data);
};

Q_DECLARE_METATYPE(QSharedPointer<IVideoData>);
Q_DECLARE_METATYPE(QSharedPointer<IAudioData>);
Q_DECLARE_METATYPE(QSharedPointer<ISubtitlesData>);

#endif // IDATASTREAM_H
