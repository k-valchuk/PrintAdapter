#include "thumbnailCreator.h"

void ThumbnailCreator::addReceiver(ThumbnailReceiver *rec)
{
    // возможно уже есть
    for (auto *r : qAsConst(receiverList))
    {
        if (r == rec)
            return;
    }

    receiverList.append(rec);
}

void ThumbnailCreator::removeReceiver(ThumbnailReceiver *rec)
{
    receiverList.removeOne(rec);
}

void ThumbnailCreator::createNextThumb()
{
    if (isWorking)//создатель в работе
        return;

    //ищем получателя если отсутствует
    if (!currentRec || currentRec->getThumbQueue().isEmpty())
        currentRec = getNextReceiver();

    if (currentRec)
    {
        isWorking = true;

        currentRec->setActive(true);//флаг ожидания

        auto thumbItem = currentRec->getThumbQueue().head().get();
        if (thumbItem)
            createThumbnail(*thumbItem);// запускаем создание миниатюры
    }
}

void ThumbnailCreator::thumbReceived(const std::shared_ptr<ThumbImageData> &thumb)
{
    isWorking = false;

    if (currentRec && currentRec->isActive())
    {//если получатель существует и ожидает
        auto itemId = currentRec->getThumbQueue().dequeue();
        //сообщаем о создании
        currentRec->thumbReceived(itemId.get(), thumb);
    }

    //запустить на создание следующую
    createNextThumb();
}

ThumbnailReceiver *ThumbnailCreator::getNextReceiver()
{
    //ищем ожидающий в очереди получатель
    for (ThumbnailReceiver *rec : qAsConst(receiverList))
    {
        if (!rec->getThumbQueue().isEmpty())
            return rec;
    }

    return nullptr;
}


/*****  ThumbnailReceiver   *****/

ThumbnailReceiver::ThumbnailReceiver() : thumbCreator(nullptr)
{
}

ThumbnailReceiver::~ThumbnailReceiver()
{
    if (thumbCreator)
        thumbCreator->removeReceiver(this);
}

void ThumbnailReceiver::setThumbCreator(ThumbnailCreator *creator)
{
    thumbCreator = creator;
    thumbCreator->addReceiver(this);
}

void ThumbnailReceiver::createThumb(const std::shared_ptr<ThumbItem> &thumbItem)
{    
    if (thumbCreator && !thumbItem->filePath.empty())
    {
        thumbQueue.append(thumbItem);//добавим в очередь
        thumbCreator->createNextThumb();//проверим на возможность создать
    }
}

void ThumbnailReceiver::setThumbFormatId(int format)
{
    thumbFormatId = format;
}

void ThumbnailReceiver::clearThumbQueue()
{
    thumbQueue.clear();//очистить очередь
    setActive(false);//сбросить флаг ожидания
}
