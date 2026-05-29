#ifndef IPRINTINTERFACE_H
#define IPRINTINTERFACE_H

#include "IModule.h"

class IPrintInterface: public IBaseInterface {
    public:
        static QUuid Id() { return QUuid::fromString(QString("12452956-cc9c-41e9-994a-b6442080b093"));}
        QUuid GetInterfaceId() override {
            return Id();
        }

        static InterfaceVersion Version() {
            return {1, 1};
        }

        InterfaceVersion GetVersion() override final {
            return Version();
        }

        virtual void OpenShowTemplate(QString subSystemId, QJsonDocument jsonDoc, QWidget* parentWidget = nullptr) = 0;

};

#endif // IPRINTINTERFACE_H