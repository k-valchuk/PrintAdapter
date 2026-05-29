#pragma once

#include "ProtocolMain.h"
#include "ProtocolBasePacket.h"

class ProtocolBase : public ProtocolMain
{
    Q_OBJECT
public:
    void connectSignalsToSocket() override;

    ~ProtocolBase();
    
	virtual void processData(const QByteArray& byteArray, int client_id = 0) = 0;

public slots:
	void send(ProtocolBasePacket *packet, int client_id);

private slots:
    void slotGotData();
};
