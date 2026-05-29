//===========================================================================================================
//		Объект QObject который запускает ISoftPlayer и ведет с ним работу
//	Проектировалось для работы в отдельном потоке. Например, для помещения в поток через moveToThread
//===========================================================================================================
#ifndef _BPLAYERDISPATCHER_H_
#define _BPLAYERDISPATCHER_H_

#include <QObject>
#include <QLibrary>

#include "ISoftPlayer.h"

#include "BramPlayerTypes.h"

namespace BramPlayer
{
    class Dispatcher : public QObject
    {
        Q_OBJECT

    public:
        explicit Dispatcher(HWND hwnd, QObject *parent = nullptr);
        ~Dispatcher();

        //------------------------------------------------------
        // Установить локальное значение кол-ва пикметров. Будет применено после перезагрузки плеера
        // Доступные значения: 0, 1, 2, 4
        // 0 - для того чтобы спрятать
        void setPeekmeterCount(int num) {	numPeekmeters = num;	}
        inline int getPeakmeterCount(void) const { return  numPeekmeters; }

        static QString parepareAudioFilePath(const QString& videoFileName);

        unsigned long pos(void);
        EPlayerState state(void);

        //=========================================
        // Прямой вызов - экспериментально !!
        bool isVideoPresent(void);

        // Звук при перемотке
        void setAudioScrub(bool bS) { bAudioScrub = bS; }
        // Звук при перемотке
        void setShowGraphics(bool bS) { bShowGraphics = bS; }
        // Звук при перемотке
        void setShowSubtitles(bool bS) { bShowSubtitles = bS; }
        // Звук при перемотке
        void setDeinterlace(bool bS) { bDeinterlace = bS; }

    private:
        //----------------------------------------
        //------Параметры подготовки файла--------
        // (параметры видео файла)(необходимо для перезагрузки плеера)
        //----------------------------------------
        // Имя видео/аудио файла который проигрываем
        QString videoPath, audioPath;
        int32_t framePrepare, frameDuration;
        //-------------------------------------
        // Кол-во пикметров в плеере
        int numPeekmeters;
        //------------------------
        //Звук при перемотке
        bool bAudioScrub;
        //------------------------
        //Отображать графику
        bool bShowGraphics;
        //------------------------
        //Отображать субтитры
        bool bShowSubtitles;
        //------------------------
        //Деинтерлейс
        bool bDeinterlace;
        //------------------------
        // Загрузка плеера из длл
        QLibrary* bramPlayerLibrary;
        bool loadLibraryAndCreate(void);
        void releaseLibraryAndDestroy(void);

        typedef void(*DllInit)(void);
        typedef void(*DllRelease)(void);
        typedef int(*DxCreatePlayer)(ISoftPlayer** ppPlayer);
        typedef int(*DxDestroyPlayer)(ISoftPlayer* pPlayer);
        typedef int(*DxCreatePlayer2)(ISoftPlayer2** ppPlayer, int playerVersion);

        DllInit dllInitFunc;
        DllRelease dllReleaseFunc;
        DxCreatePlayer2 dxCreatePlayer2;
        DxDestroyPlayer dxDestroyPlayer;

        // Интерфейс для работы с плеером
        ISoftPlayer2* softPlayer2;
        // Последний статус
        SSoftPlayerState lastState;

        int statePlayerTimerID;

        // Храним для перезагрузки
        HWND hwnd;

        EPlayerState getPlayerState(unsigned int);
        //---------------
        bool appendClip(const QString& videoPath, const QString& audioPath, int32_t frameDuration = -1, int32_t framePrepare = 0);

        EPlayerState private_state(void);
        unsigned long private_pos(void);

        void timerEvent(QTimerEvent* event);

    public slots:
        void init(void);

        // Установить частоту работы плеера
        bool setPlayerRate(unsigned int rate, unsigned int scale);

        void setPlayerSize(BramPlayer::EPleerSize size);
        // Другие параметры плеера
        void setIsKeepAspectRatioFrame(bool is);

        // Добавить(установить) проигрываемый клип
        bool appendClip(QString path, int32_t frameDuration = -1, int32_t framePrepare = 0);

        // Очистить данные плеера
        void clear(void);

        // Перезагрузка плеера с сохранением статусов
        // При перезагрузки плеера мы выбираем нужно ли сохранять статус который был до перезагрузки или нет(установить в паузу)
        // Делаем это из-за того что в некоторых моментах происходит ошибка восстановления статуса (последоветльность seek(p)->play=seek(0)->play )
        // (Иначе бы сделали по умолчанию - да)
        bool reload(bool isRecoverState = false);
        // Перезагрузка плеера с обновленной длительностью
        bool reload(unsigned long frameDuration, bool isRecoverState = false);

        //--------------------------------------------------------------------------------------
        // Отстановка/запуск таймера обратной связи с плеером
        //-----------------
        // В таймере происходит опрос состояния плеера, позиции проигрывания и тд.
        // При работе таймера происходит проверка на изменение статуса и позиции, а также
        //  если эти параметры поменялись - вызов сигналов posChanged или stateChanged.
        // Данные методы вынесены как public т.к. иногда удобно отключать уведомление о изменении статусе воспроизведения
        void stopFeedbackTimer(void);
        void startFeedbackTimer(void);
        //--------------------------------------------------------------------------------------
        // Элементарные команды управления
        bool play(void);
        bool pause(void);
        bool stop(void);

        // Переход к кадру
        bool seek(unsigned int pos);
        bool seek(int pos);

        // Принудительный рендер
        void repaint(void);

    signals:
        // Изменение статуса воспроизведения
        void posChanged(int pos);
        void stateChanged(EPlayerState state);

        // Сообщение о проблемеы (при инициализции и тд)
        void problem(const QString &message);
        // Клип подготовлен
        void filePrepared(bool isSuccessfull);
        // Закончена очистка плеера
        void cleared(void);
    };

}


#endif // _BPLAYERDISPATCHER_H_
