#ifndef IVIDEOPLAYERINTERFACE_H
#define IVIDEOPLAYERINTERFACE_H

#include "IModule.h"
#include "IModuleWidget.h"
#include <QSharedPointer>
#include <functional>
#include <QMimeData>
#include <variant>

class IProjectItem;
class IVideoPlayerProject;
class IVideoPlayerContent;

class IVideoPlayer
{
public:
    
    struct GraphicsPosition
    {
        std::variant<int64_t, QDateTime> Origin = (int64_t)0;
        int64_t StartPos = 0;
        int64_t Duration = 0;
    };
    
    struct Graphic
    {
        QString FilePath;
        
        GraphicsPosition Position;
        
        int PosX = 0;
        int PosY = 0;
    };
    
    struct Subtitle
    {
        QString FilePath;
        QString TrackKey;
    };
    
    struct SubtitleTrackInfo
    {
        QString TrackKey;
        QString PrettyName;
    };
    
    struct SimpleContent
    {
        QString VideoFilePath;
        QString AudioFilePath;
        QString PlayerTitle;
        
        QMap<int, Graphic> Graphics;
        QVector<Subtitle> SubtitleTracks;
    };
    
    struct Object
    {
        QString  ObjectType;
        QVariant ObjectId;
    };
    
    struct Message
    {
        QString Title;
        QString Text;
    };
    
    enum ActionButtonPosition
    {
        Left = 1,
        Right = 2
    };
    
    struct ActionCallbackData
    {
        int64_t In, Out;
        QDateTime InDt, OutDt;
        SimpleContent Content;
        QWidget* Button;
    };
    
    struct TimeDelayInOutData
    {
        QDateTime InDt, OutDt;
        int64_t In, Out;
    };
    
    //---------------------------------------------------------------------------
    
    virtual void SetContent(const SimpleContent& content) = 0;
    
    virtual QSharedPointer<IVideoPlayerContent> CreateContent() = 0;
    virtual void SetContent(QSharedPointer<IVideoPlayerContent> content) = 0;
    
    //будет искать хендлер для объекта
    virtual void SetObject(const Object& id) = 0;
    
    virtual QSharedPointer<IVideoPlayerProject> CreateProject() = 0;
    virtual void SetProject(QSharedPointer<IVideoPlayerProject> proj) = 0;
    
    virtual void SetAvailiableSubtitleTracks(const QVector<SubtitleTrackInfo>& tracks) = 0;
    
    virtual void MoveGraphics(int layer, const GraphicsPosition& npos) = 0;
    
    //---------------------------------------------------------------------------
    
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void SetRange(int64_t left, int64_t right) = 0;
    virtual void ResetRange() = 0;
    virtual void SetLoop(bool loop) = 0;
    virtual void Clean() = 0;
    
    virtual void SetMessage(const Message& message) = 0;
    
    virtual void SetCurrentTc(int64_t tc) = 0;
    
    virtual void SubscribeOnCurrentTcChanged(
        std::function<void(int64_t)> func,
        QObject* context = nullptr
    ) = 0;
    
    virtual void SetInOutOrigin(std::variant<int64_t, QDateTime> origin) = 0;
    virtual void SetIn(int64_t tc) = 0;
    virtual void SetOut(int64_t tc) = 0;
    
    virtual void SetTimelineScale(double left, double right) = 0;
    
    virtual void SubscribeOnInOutChanged(
        std::function<void(int64_t, int64_t, bool)> func,
        QObject* context = nullptr
    ) = 0;
    
    virtual void SubscribeToSeparatorsChanged(
        std::function<void(const QVector<int64_t>&)> func,
        QObject* context = nullptr
    ) = 0;
    
    virtual void SubscribeOnStartEndChanged(
        std::function<void(int64_t, int64_t)> func,
        QObject* context = nullptr
    ) = 0;
    
    virtual void SubscribeOnFramerateChanged(
        std::function<void(int, int)> func,
        QObject* context = nullptr
        ) = 0;
    
    //-------------------- колеченое апи
    
    virtual void SetCurrentDt(const QDateTime& dt) = 0;
    
    virtual void SubscribeOnCurrentDtChanged(
        std::function<void(int64_t, const QDateTime&)> func,
        QObject* context = nullptr
        ) = 0;
    
    virtual void SetInDt(const QDateTime& inDt) = 0;
    virtual void SetOutDt(const QDateTime& outDt) = 0;
    
    virtual void SubscribeOnInOutDtChanged(
        std::function<void(const TimeDelayInOutData&, bool)> func,
        QObject* context = nullptr
        ) = 0;
    
    virtual void SetTimelineScale(const QDateTime& leftDt, const QDateTime& rightDt) = 0;
    
    //----------------------------
    virtual void AddAction(const QString& key, const QString& svgIconPath, ActionButtonPosition pos) = 0;
    virtual void RemoveAction(const QString& key) = 0;
    virtual void SubscribeOnActionClicked(std::function<void(const QString&, const ActionCallbackData&)> callback,
                                          QObject* context = nullptr) = 0;
};

class IVideoPlayerContent
{
public:
    virtual void SetVideoFilePath(const QString& videoPath) = 0;
    virtual void SetAudioFilePath(const QString& audioPath) = 0;
    
    virtual void SetPlayerTitle(const QString& title) = 0;
    virtual void SetError(const QString& errorMessage) = 0;
    
    virtual void AddGraphics(int layer, const IVideoPlayer::Graphic& graphics) = 0;
    virtual void RemoveGraphics(int layer) = 0;
    
    virtual void AddSubtitleTrack(const IVideoPlayer::Subtitle& subt) = 0;
    virtual void RemoveSubtitleTrack(const QString& trackKey) = 0;
    
    //начало и конец в файле (в семплах файла)
    //в случае использования IVideoPlayerContent как одного объекта
    //in и out будут использованы как установленные метки
    //а в случае использования в проекте - как начало и конец куска с этим контентом
    //
    virtual void SetIn (int64_t  in) = 0; //меньше 0 - будет с начала файла
    virtual void SetOut(int64_t out) = 0; //меньше 0 - будет до конца файла
    virtual void SetDuration(int64_t dur) = 0; //будет использован если не задан out или если это кольцо
    virtual void SetInDateTime(const QDateTime& inDt) = 0;
    virtual void SetOutDateTime(const QDateTime& outDt) = 0;
};

class IProjectItem
{
public:
    //начало на таймлайне (в семплах таймлайна, частота указывается в IVideoPlayerProject)
    virtual void SetStart(int64_t start) = 0; //меньше 0 - будет в стык за предыдущем итемом
    
    //начало и конец в файле (в семплах файла)
    virtual void SetIn (int64_t  in) = 0; //меньше 0 - будет с начала файла
    virtual void SetOut(int64_t out) = 0; //меньше 0 - будет до конца файла
    virtual void SetDuration(int64_t dur) = 0; //будет использован если не задан out или если это кольцо
    virtual void SetInDateTime (const QDateTime& inDt ) = 0;
    virtual void SetOutDateTime(const QDateTime& outDt) = 0;
    
    //не всё сразу
    //virtual void SetAutoUpdateFileOut(bool) = 0;
    
    virtual void SetContent(const IVideoPlayer::SimpleContent& content) = 0;
    virtual void SetObject(const IVideoPlayer::Object& object, bool respectObjectParameters = true) = 0;
};

class IVideoPlayerProject
{
public:
    //частота кадров таймлайна
    virtual void SetTimelineRate(int rate) = 0;
    
    virtual QSharedPointer<IProjectItem> CreateItem() = 0;
    
    virtual void AddItem(QSharedPointer<IProjectItem> item) = 0;
    virtual void RemoveItem(IProjectItem* item) = 0;
};

class IVideoPlayerWidget
{
public:
    virtual IModuleWidget* GetPanel() = 0;
    virtual QSharedPointer<IVideoPlayer> GetPlayer() = 0;
    
    virtual void SetMimeDataHandler(std::function<bool(const QMimeData*)> dragHandler,
                                    std::function<void(const QMimeData*, QSharedPointer<IVideoPlayer>)> dropHandler) = 0;
};


class IVideoPlayerInterface : public IBaseInterface
{
public:
    static QUuid Id()
    {
        return QUuid::fromString(QString("d3c0d00f-eae0-4a9f-998c-bf7cc402704d"));
    }
    
    QUuid GetInterfaceId() override final
    {
        return Id();
    }
    
    static InterfaceVersion Version()
    {
        return {1, 13};
    }
    
    InterfaceVersion GetVersion() override final
    {
        return Version();
    }
    
    //создает но не открывает его сам
    virtual IVideoPlayerWidget* CreateVideoPlayerPanel() = 0;
    
    //ищет открытую или открывает если стоит флаг, может вернуть nullptr!
    virtual QSharedPointer<IVideoPlayer> GetVideoPlayer(bool openIfNotExists = true) = 0;
    
    //позволяет зарегать обработчик каких-то мимеДат, которые дропнули на плеер
    //на конкретные действия которые плееру надо сделать например SetFile(path)
    //dragHandler вызывается при наведении, надо вернуть true, если вы хотите работать с данным объектом
    //и тогда для вас вызовется dropHandler с указателем на плеер
    virtual void RegisterMimeDataHandler(std::function<bool(const QMimeData*)> dragHandler,
                                         std::function<void(const QMimeData*, QSharedPointer<IVideoPlayer>)> dropHandler) = 0;
    
    //позволяет зарегать обработчик каких-то объектов, которые могут задать через IVideoPlayer::SetObject(QVariant id)
    //верните из checker true если вы хотите и готовы обработать данный тип объекта QVariant id
    //обработчик handler должен заполнить IVideoPlayerContent файлом и по необходимости параметрами
    //вполне нужно делать обработчик handler блокирующим, выполняться все равно будет не в гуи потоке, а на очереди плеера
    virtual void RegisterObjectHandler(std::function<bool(const IVideoPlayer::Object&)> checker,
                                       std::function<void(const IVideoPlayer::Object&, QSharedPointer<IVideoPlayerContent>)> handler) = 0;
    
    //для преобразования произвольного объекта из формата передачи объектов BramSpace (#25796)
    //в IProjectItem, указатель на него передан для заполнения в функтор handler
    //в нем вы можете задать Object или Content и настроить параметры (in, out)
    virtual void RegisterBramSpaceObjectFormatHandler(
        std::function<bool(const QJsonObject&)> checker,
        std::function<void(const QJsonObject&, QSharedPointer<IProjectItem>)> handler
    ) = 0;
    
    //добавить объект для плеера в мимеДата
    virtual void AddObjectToMimeData(QMimeData* mimeData, const IVideoPlayer::Object& object) = 0;
    
    //подписаться на задание любыми другими модулями объектов в плеер через SetObject
    //вы получаете указатель на IVideoPlayer без отбора владения у того кто его получал в изначальном месте
    //и можете использовать только внутри callback
    //например можно добавить новый экшон и подписка на его обработку
    // - таким образом Air добавляет кнопку "экспорта" в плейлист, и это работает для всех кто сделал
    // player->SetObject("storage", {storageId, objectId, ...})
    virtual void SubscribeToObjectSet(std::function<void(const IVideoPlayer::Object&, IVideoPlayer*)> callback) = 0;
};



#endif // IVIDEOPLAYERINTERFACE_H
