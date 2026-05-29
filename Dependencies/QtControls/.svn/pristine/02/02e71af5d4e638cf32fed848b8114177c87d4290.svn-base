#include "IDataStream.h"

IDataStream::IDataStream(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<QSharedPointer<IVideoData>>();
    qRegisterMetaType<QSharedPointer<IAudioData>>();
    qRegisterMetaType<QSharedPointer<ISubtitlesData>>();
}
