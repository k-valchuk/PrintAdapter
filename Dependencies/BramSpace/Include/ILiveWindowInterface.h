#ifndef ILIVEWINDOWINTERFACE_H
#define ILIVEWINDOWINTERFACE_H

#include "IModule.h"
#include "IModuleWidget.h"


class ILiveWindowWidget : public IModuleWidget
{
public:
    ILiveWindowWidget(IModule* module) : IModuleWidget(module) {}
    
    virtual void SetUrl(QString rtspUrl) = 0;
    virtual void SetTitle(QString title) = 0;
    
    enum class SoundMode
    {
        On,
        Off,
        OnWhenVisible
    };
    
    virtual void SetMuteWhenNotVisible(bool muteWhenNotVisible) = 0;
};


// Интерфейс работы с окнами отображения rtsp потоков
class ILiveWindowInterface : public IBaseInterface
{
public:
    static QUuid Id()
    {
        return QUuid::fromString(QString("7cd92acd-ea89-4586-a018-93a642e63dd3"));
    }
    
    QUuid GetInterfaceId() override final
    {
        return Id();
    }
    
    static InterfaceVersion Version()
    {
        return {1, 2};
    }
    
    InterfaceVersion GetVersion() override final
    {
        return Version();
    }
    
    //возвращает указатель на октрываемый ILiveWindowWidget* видеоплеера (который является IModuleWidget*)
    //(например, чтобы по нему можно было потом его закрыть через WindowManager)
    virtual ILiveWindowWidget* OpenLiveWindowAndShowStream() = 0;
    
    //создает но не открывает его сам
    virtual ILiveWindowWidget* CreateLiveWindow() = 0;
};

#endif // ILIVEWINDOWINTERFACE_H
