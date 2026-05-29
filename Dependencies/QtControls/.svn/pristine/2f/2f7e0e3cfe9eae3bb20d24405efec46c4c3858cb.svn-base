//-------------------------------------------------------------------------------------------------
// Создание миниатюр
//-------------------------------------------------------------------------------------------------
//
// У Создателя есть список Получателей
// У каждого Получателя своя очередь миниатюр
// Очистка очереди через clearQueue(), сбрасывает флаг ожидании миниатюры
//
//-------------------------------------------------------------------------------------------------
//  *** Создание своего ThumbnailCreator ***
//
//  Переопределяем метод создания миниатюры createThumbnail.
//  После создания миниатюры вызываем метод thumbReceived (метод отправит созданную миниатюру получателю
//      и перейдет к созданию следующей в очереди).
//-------------------------------------------------------------------------------------------------
#ifndef THUMBNAILCREATOR_H
#define THUMBNAILCREATOR_H

#include "ThumbStructs.h"
#include "QQueue"
#include "QVector"
#include "QObject"
#include <memory>

class ThumbnailCreator;

// Получатель миниатюр
class ThumbnailReceiver
{
public:
    ThumbnailReceiver();
    virtual ~ThumbnailReceiver();

    // Получена миниатюра
    virtual void thumbReceived(ThumbItem *item, const std::shared_ptr<ThumbImageData> &thumb) = 0;

    // Задать создателя
    void setThumbCreator(ThumbnailCreator *creator);

    // Создать миниатюру
    void createThumb(const std::shared_ptr<ThumbItem> &thumbItem);

    // Получить очередь
    QQueue<std::shared_ptr<ThumbItem> >& getThumbQueue() {return thumbQueue;}

    // Очистить очередь
    void clearThumbQueue();

    // Id формата миниатюры
    void setThumbFormatId(int format);
    int getThumbFormatId() const {return thumbFormatId;}

    // Статус активности получателя
    bool isActive() const {return bIsActive;}
    void setActive(bool bActive) {bIsActive = bActive;}

private:
    ThumbnailCreator *thumbCreator;                 // Создатель миниатюр
    int thumbFormatId = -1;                         // Id формата миниатюры
    QQueue<std::shared_ptr<ThumbItem> > thumbQueue;      // Очередь создания миниатюр
    bool bIsActive = false;                         // Статус активности (ожидает миниатюру)
};

// Создатель миниатюр
class ThumbnailCreator :public QObject
{
public:
    ThumbnailCreator(){}
    virtual ~ThumbnailCreator(){}

    void addReceiver(ThumbnailReceiver *rec);       // Добавить получателя
    void removeReceiver(ThumbnailReceiver *rec);    // Удалить получателя
    void createNextThumb();                         // Создать следующую миниатюру из очереди

    // Миниатюра получена
    void thumbReceived(const std::shared_ptr<ThumbImageData> &thumb);

protected:
    // Реализация создания миниатюры
    virtual void createThumbnail(const ThumbItem &thumbItem) = 0;

    ThumbnailReceiver *currentRec = nullptr;                    // Текущий получатель

private:
    QVector<ThumbnailReceiver *> receiverList;                  // Список получателей
    bool isWorking = false;                                     // В работе (создается миниатюра)

    ThumbnailReceiver* getNextReceiver();                       // Следующий получатель из очереди
};

#endif // THUMBNAILCREATOR_H
