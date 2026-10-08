#ifndef IVIDEOPLAYERINTERFACE_H
#define IVIDEOPLAYERINTERFACE_H

#include "IModule.h"
#include "IModuleWidget.h"

#include <QMap>
#include <QMimeData>
#include <QSharedPointer>
#include <QVector>

#include <functional>
#include <optional>
#include <variant>

class IVideoPlayerContent;
class IVideoPlayerObjectResult;
class IVideoPlayerGraphicsInstance;
class IVideoPlayerSubtitleInstance;
class IVideoPlayerClip;
class IVideoPlayerItem;
class IVideoPlayerProject;

class IVideoPlayer
{
public:
    struct GraphicsPosition
    {
        std::variant<int64_t, QDateTime> Origin = int64_t{0};
        int64_t StartPos = 0;
        int64_t Duration = -1; //меньше 0 - возьмет всю длительность по файлу шаблона
    };

    struct GraphicParameter
    {
        QString Name;
        QString Value;
    };

    struct Graphic
    {
        QString FilePath;
        GraphicsPosition Position;
        int PosX = 0;
        int PosY = 0;
        QVector<GraphicParameter> Parameters;
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

    struct ContentParameters
    {
        // 0 / <= 0 означает "не задано":
        // - если есть видео, значения будут взяты из видео;
        // - если видео нет, будут использованы дефолты.
        int FrameRate = 0;
        int FrameScale = 1;

        int FrameWidth = 0;
        int FrameHeight = 0;

        int AspectRatioNum = 0;
        int AspectRatioDen = 0;

        // длительность таймлайна в кадрах.
        // если <= 0 и видео нет, будет вычислена по графике/аудио.
        int64_t Duration = 0;
    };

    // уже готовый контент для плеера: пути к файлам и всё, что надо отрисовать.
    // in/out/duration сюда специально не входят - они относятся к конкретному клипу.
    struct SimpleContent
    {
        QString VideoFilePath;
        QString AudioFilePath;
        QString PlayerTitle;

        QMap<int, Graphic> Graphics;
        QVector<Subtitle> SubtitleTracks;

        ContentParameters Parameters;
    };

    // ссылка на объект произвольной подсистемы;
    // как его получить знает зарегистрированный ObjectHandler.
    struct Object
    {
        QString ObjectType;
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
        std::optional<int64_t> In, Out;
        QDateTime InDt, OutDt;
        SimpleContent Content;
        QWidget* Button = nullptr;
    };

    struct InOutData
    {
        QDateTime InDt, OutDt;
        std::optional<int64_t> In, Out;
        int FrameRate = 25;
        int FrameScale = 1;
    };

    // политика ошибки при разрешении дополнительного Object внутри Clip.
    // Optional - пропустить этот источник и продолжить собирать Clip;
    // Required - ошибка этого источника считается ошибкой всего Clip.
    enum class ResolveFailurePolicy
    {
        Optional,
        Required
    };

    //-----------------------------------------------------------------------
    // контент / клипы

    virtual QSharedPointer<IVideoPlayerContent> CreateContent() = 0;
    virtual QSharedPointer<IVideoPlayerClip> CreateClip() = 0;

    // основной способ задать содержимое плеера.
    // ObjectHandler'ы (в том числе потенциально блокирующие) будут вызваны внутри очереди плеера.
    virtual void SetClip(QSharedPointer<IVideoPlayerClip> clip) = 0;

    // короткие варианты для простых случаев, внутри всё равно сводятся к SetClip().
    virtual void SetContent(const SimpleContent& content) = 0;
    virtual void SetContent(QSharedPointer<IVideoPlayerContent> content) = 0;
    virtual void SetObject(const Object& object) = 0;

    virtual QSharedPointer<IVideoPlayerProject> CreateProject() = 0;
    virtual void SetProject(QSharedPointer<IVideoPlayerProject> project) = 0;

    virtual void SetAvailiableSubtitleTracks(const QVector<SubtitleTrackInfo>& tracks) = 0;
    virtual void MoveGraphics(int layer, const GraphicsPosition& npos) = 0;

    //-----------------------------------------------------------------------

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
        QObject* context = nullptr) = 0;

    virtual void SetInOutOrigin(std::variant<int64_t, QDateTime> origin) = 0;
    virtual void SetIn(std::optional<int64_t> tc) = 0;
    virtual void SetOut(std::optional<int64_t> tc) = 0;
    virtual void SetInOut(std::optional<int64_t> in, std::optional<int64_t> out) = 0;

    virtual void SetInOutsClearingEnabled(bool enableClearing) = 0;
    virtual void SetTimelineScale(double left, double right) = 0;

    virtual void SubscribeOnInOutChanged(
        std::function<void(std::optional<int64_t>, std::optional<int64_t>, bool)> func,
        QObject* context = nullptr) = 0;
    virtual void SubscribeOnInChanged(
        std::function<void(std::optional<int64_t>, bool)> func,
        QObject* context = nullptr) = 0;
    virtual void SubscribeOnOutChanged(
        std::function<void(std::optional<int64_t>, bool)> func,
        QObject* context = nullptr) = 0;

    virtual void SubscribeToSeparatorsChanged(
        std::function<void(const QVector<int64_t>&)> func,
        QObject* context = nullptr) = 0;
    virtual void SubscribeOnStartEndChanged(
        std::function<void(int64_t, int64_t)> func,
        QObject* context = nullptr) = 0;
    virtual void SubscribeOnFramerateChanged(
        std::function<void(int, int)> func,
        QObject* context = nullptr) = 0;

    //-------------------- API для колец / date-time
    virtual void SetCurrentDt(const QDateTime& dt) = 0;
    virtual void SubscribeOnCurrentDtChanged(
        std::function<void(int64_t, const QDateTime&)> func,
        QObject* context = nullptr) = 0;

    virtual void SetInDt(const QDateTime& inDt) = 0;
    virtual void SetOutDt(const QDateTime& outDt) = 0;
    virtual void SetInOutDt(const QDateTime& inDt, const QDateTime& outDt) = 0;

    virtual void SubscribeOnInOutDtChanged(
        std::function<void(const InOutData&, bool)> func,
        QObject* context = nullptr) = 0;
    virtual void SubscribeOnInDtChanged(
        std::function<void(std::optional<int64_t>, const QDateTime&, bool)> func,
        QObject* context = nullptr) = 0;
    virtual void SubscribeOnOutDtChanged(
        std::function<void(std::optional<int64_t>, const QDateTime&, bool)> func,
        QObject* context = nullptr) = 0;

    virtual void SetTimelineScale(const QDateTime& leftDt, const QDateTime& rightDt) = 0;

    virtual void AddAction(const QString& key, const QString& svgIconPath, ActionButtonPosition pos) = 0;
    virtual void RemoveAction(const QString& key) = 0;
    virtual void SubscribeOnActionClicked(
        std::function<void(const QString&, const ActionCallbackData&)> callback,
        QObject* context = nullptr) = 0;

    virtual void AddMenuAction(const QString& key, const QString& name) = 0;
    virtual void RemoveMenuAction(const QString& key) = 0;

    virtual QWidget* PlayerParentWidget() = 0;
};

// уже разрешенный контент: конкретные файлы и данные, которые плеер может открыть/отрисовать.
// in/out/duration здесь специально нет - они зависят от использования контента в конкретном клипе.
class IVideoPlayerContent
{
public:
    virtual ~IVideoPlayerContent() = default;

    virtual void SetVideoFilePath(const QString& videoPath) = 0;
    virtual void SetAudioFilePath(const QString& audioPath) = 0;

    virtual void SetPlayerTitle(const QString& title) = 0;
    virtual void SetError(const QString& errorMessage) = 0;

    virtual void AddGraphics(int layer, const IVideoPlayer::Graphic& graphics) = 0;
    virtual void SetGraphicsParameter(int layer, const IVideoPlayer::GraphicParameter& param) = 0;
    virtual void RemoveGraphics(int layer) = 0;

    virtual void AddSubtitleTrack(const IVideoPlayer::Subtitle& subt) = 0;
    virtual void RemoveSubtitleTrack(const QString& trackKey) = 0;

    virtual void SetContentParameters(const IVideoPlayer::ContentParameters& params) = 0;
    virtual void SetMimeDataGetter(
        std::function<QMimeData*(const IVideoPlayer::InOutData&)> mimeGetter) = 0;

    virtual QString VideoFilePath() const = 0;
    virtual QString AudioFilePath() const = 0;
    virtual QString PlayerTitle() const = 0;
    virtual QString ErrorMessage() const = 0;

    virtual QMap<int, IVideoPlayer::Graphic> GraphicsList() const = 0;
    virtual QVector<IVideoPlayer::Subtitle> SubtitlesList() const = 0;
    virtual IVideoPlayer::ContentParameters AdditionalContentParameters() const = 0;
};

// результат, который заполняет ObjectHandler.
// Content - что физически открыть/отрисовать.
// Default* - стандартные in/out/duration самого объекта; Clip при необходимости может их переопределить.
class IVideoPlayerObjectResult
{
public:
    virtual ~IVideoPlayerObjectResult() = default;

    virtual QSharedPointer<IVideoPlayerContent> Content() = 0;

    virtual void SetDefaultIn(std::optional<int64_t> in) = 0;
    virtual void SetDefaultOut(std::optional<int64_t> out) = 0;
    virtual void SetDefaultDuration(std::optional<int64_t> duration) = 0;
    virtual void SetDefaultInDateTime(const QDateTime& inDt) = 0;
    virtual void SetDefaultOutDateTime(const QDateTime& outDt) = 0;
};

// настройки конкретного экземпляра графики в клипе.
// Object/Graphic задает саму графику, а здесь задаются положение, координаты и параметры
// именно для этого её использования.
class IVideoPlayerGraphicsInstance
{
public:
    virtual ~IVideoPlayerGraphicsInstance() = default;

    virtual void SetPosition(const IVideoPlayer::GraphicsPosition& position) = 0;
    virtual void SetPosX(int x) = 0;
    virtual void SetPosY(int y) = 0;
    virtual void SetParameter(const IVideoPlayer::GraphicParameter& parameter) = 0;
};

// настройки конкретного экземпляра субтитров.
// пока здесь только переопределение TrackKey, но отдельный интерфейс оставлен,
// чтобы потом можно было расширить его без очередной переделки API.
class IVideoPlayerSubtitleInstance
{
public:
    virtual ~IVideoPlayerSubtitleInstance() = default;
    virtual void SetTrackKey(const QString& trackKey) = 0;
};

// один логический клип для плеера.
// содержит основной источник (Content/Object), свой in/out и наложенную графику/субтитры.
// позиция на таймлайне проекта сюда не относится - она задается у IVideoPlayerItem.
class IVideoPlayerClip
{
public:
    virtual ~IVideoPlayerClip() = default;

    virtual void SetContent(const IVideoPlayer::SimpleContent& content) = 0;
    virtual void SetContent(QSharedPointer<IVideoPlayerContent> content) = 0;
    virtual void SetObject(const IVideoPlayer::Object& object) = 0;

    // если после ObjectHandler надо дополнительно поправить получившийся контент.
    // вызывается после разрешения основного объекта, но до добавления графики/субтитров Clip.
    // выполняется внутри очереди плеера.
    virtual void SetResolvedContentEditor(
        std::function<void(QSharedPointer<IVideoPlayerContent>)> editor) = 0;

    virtual QSharedPointer<IVideoPlayerGraphicsInstance> AddGraphics(
        int layer, const IVideoPlayer::Graphic& graphics) = 0;

    // Object-вторички по умолчанию необязательные: если resolver вернет ошибку,
    // источник будет пропущен. Required позволяет сделать конкретную вторичку обязательной.
    virtual QSharedPointer<IVideoPlayerGraphicsInstance> AddGraphicsObject(
        int layer,
        const IVideoPlayer::Object& object,
        IVideoPlayer::ResolveFailurePolicy failurePolicy = IVideoPlayer::ResolveFailurePolicy::Optional) = 0;
    virtual void RemoveGraphics(int layer) = 0;

    virtual QSharedPointer<IVideoPlayerSubtitleInstance> AddSubtitleTrack(
        const IVideoPlayer::Subtitle& subtitle) = 0;
    virtual QSharedPointer<IVideoPlayerSubtitleInstance> AddSubtitleObject(
        const QString& trackKey,
        const IVideoPlayer::Object& object,
        IVideoPlayer::ResolveFailurePolicy failurePolicy = IVideoPlayer::ResolveFailurePolicy::Optional) = 0;
    virtual void RemoveSubtitleTrack(const QString& trackKey) = 0;

    // явно заданные здесь значения имеют приоритет над Default* из ObjectHandler.
    // при задании числового In/Out очищается соответствующий DateTime и наоборот.
    virtual void SetIn(std::optional<int64_t> in) = 0;
    virtual void SetOut(std::optional<int64_t> out) = 0;
    virtual void SetDuration(std::optional<int64_t> duration) = 0;
    virtual void SetInDateTime(const QDateTime& inDt) = 0;
    virtual void SetOutDateTime(const QDateTime& outDt) = 0;
};

// размещение клипа в проекте.
// сам контент и его in/out лежат в IVideoPlayerClip, здесь только позиция на таймлайне.
class IVideoPlayerItem
{
public:
    virtual ~IVideoPlayerItem() = default;

    // nullopt / отрицательное значение - поставить встык за предыдущим item.
    virtual void SetStart(std::optional<int64_t> start) = 0;

    virtual QSharedPointer<IVideoPlayerClip> Clip() = 0;
    virtual void SetClip(QSharedPointer<IVideoPlayerClip> clip) = 0;
};

class IVideoPlayerProject
{
public:
    virtual ~IVideoPlayerProject() = default;

    virtual void SetTimelineRate(int rate) = 0;

    virtual QSharedPointer<IVideoPlayerItem> CreateItem() = 0;
    virtual void AddItem(QSharedPointer<IVideoPlayerItem> item) = 0;
    virtual void RemoveItem(IVideoPlayerItem* item) = 0;
};

class IVideoPlayerWidget
{
public:
    virtual ~IVideoPlayerWidget() = default;

    virtual IModuleWidget* GetPanel() = 0;
    virtual QSharedPointer<IVideoPlayer> GetPlayer() = 0;

    struct DefaultPanelParameters
    {
        bool EnableInOutsClearing = true;
        std::variant<int64_t, QDateTime> InOutOririn = int64_t{0};
    };

    virtual void SetDefaultPanelParameters(const DefaultPanelParameters& params) = 0;

    virtual void SetMimeDataHandler(
        std::function<bool(const QMimeData*)> dragHandler,
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

    // версия 2: новое API с IVideoPlayerClip и отдельным разрешением Object -> Content.
    static InterfaceVersion Version()
    {
        return {2, 0};
    }

    InterfaceVersion GetVersion() override final
    {
        return Version();
    }

    virtual IVideoPlayerWidget* CreateVideoPlayerPanel() = 0;
    virtual QSharedPointer<IVideoPlayer> GetVideoPlayer(bool openIfNotExists = true) = 0;
    
    //позволяет зарегать обработчик каких-то мимеДат, которые дропнули на плеер
    //на конкретные действия которые плееру надо сделать например SetFile(path)
    //dragHandler вызывается при наведении, надо вернуть true, если вы хотите работать с данным объектом
    //и тогда для вас вызовется dropHandler с указателем на плеер
    virtual void RegisterMimeDataHandler(
        std::function<bool(const QMimeData*)> dragHandler,
        std::function<void(const QMimeData*, QSharedPointer<IVideoPlayer>)> dropHandler) = 0;

    //позволяет зарегать обработчик каких-то объектов, которые могут задать через IVideoPlayer::SetObject({type, id})
    //верните из checker true если вы хотите и готовы обработать данный тип объекта QVariant id
    //обработчик handler должен заполнить IVideoPlayerContent файлом и по необходимости параметрами
    //вполне нужно делать обработчик handler блокирующим, выполняться все равно будет не в гуи потоке, а на очереди плеера
    virtual void RegisterObjectHandler(
        std::function<bool(const IVideoPlayer::Object&)> checker,
        std::function<void(const IVideoPlayer::Object&, QSharedPointer<IVideoPlayerObjectResult>)> handler) = 0;
    
    //для преобразования произвольного объекта из формата передачи объектов BramSpace (#25796)
    //в IVideoPlayerClip, указатель на него передан для заполнения в функтор handler
    //в нем вы можете задать Object или Content, добавить вторичные объекты/файлы и настроить параметры (in, out)
    virtual void RegisterBramSpaceObjectFormatHandler(
        std::function<bool(const QJsonObject&)> checker,
        std::function<void(const QJsonObject&, QSharedPointer<IVideoPlayerClip>)> handler) = 0;

    virtual void AddObjectToMimeData(QMimeData* mimeData, const IVideoPlayer::Object& object) = 0;
    
    //подписаться на задание любыми другими модулями объектов в плеер через SetObject
    //вы получаете указатель на IVideoPlayer без отбора владения у того кто его получал в изначальном месте
    //и можете использовать только внутри callback
    //например можно добавить новый экшон и подписка на его обработку
    // - таким образом Air добавляет кнопку "экспорта" в плейлист, и это работает для всех кто сделал
    // player->SetObject("storage", {storageId, objectId, ...})
    virtual void SubscribeToObjectSet(
        std::function<void(const IVideoPlayer::Object&, IVideoPlayer*)> callback) = 0;
};

#endif // IVIDEOPLAYERINTERFACE_H
