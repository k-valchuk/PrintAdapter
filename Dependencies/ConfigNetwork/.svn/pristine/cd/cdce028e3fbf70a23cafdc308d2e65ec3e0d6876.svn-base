#ifndef CONFIGSUBSCRIBERNOTIFIER_H
#define CONFIGSUBSCRIBERNOTIFIER_H

#include <SubscribersNotifier.h>
#include <ConfigStructs.h>

class ConfigSubscriberNotifier : public SubscribersNotifier {

public:
    ConfigSubscriberNotifier(QString module = "core") : SubscribersNotifier::SubscribersNotifier(), m_module(module) {};
    QString getModule(){return m_module;};
private:
    const QString getPath(void) const final
    {
        return QString("/api/%1/config/notify").arg(m_module);
    }

    QString m_module = "core";
};

#endif // CONFIGSUBSCRIBERNOTIFIER_H
