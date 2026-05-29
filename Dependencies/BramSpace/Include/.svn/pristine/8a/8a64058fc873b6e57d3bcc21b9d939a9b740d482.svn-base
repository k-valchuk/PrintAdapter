#ifndef IMODULEWITHHOTKEYS_H
#define IMODULEWITHHOTKEYS_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QKeySequence>
#include <QUuid>

class IModule;
class IModuleWithHotkeys
{ //от него могут быть унаследованы модули, для виджетов которых нужна настройка хоткеев
public:
    
    struct Action
    {
        QString Key;
        std::function<QString()> Name;
        QKeySequence Sequence;
    };
    
    struct Widget
    {
        int WidgetType;
        std::function<QString()> Name;
    };
    
    virtual QMap<Widget, QVector<Action>> GetHotkeyActions() = 0;
    virtual void OnHotkeysChanged(int widgetId, QString key, QKeySequence newKeySequence) = 0;
};

inline bool operator <(const IModuleWithHotkeys::Widget &w1, const IModuleWithHotkeys::Widget &w2)
{
    if(w1.Name() == w2.Name())
        return w1.WidgetType < w2.WidgetType;
    return w1.Name() < w2.Name();
}

#endif // IMODULEWITHHOTKEYS_H
