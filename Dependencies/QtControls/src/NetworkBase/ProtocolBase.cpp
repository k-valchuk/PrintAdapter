#include "ProtocolBase.h"

//-------------------------------------------------------------------------------------------------
void ProtocolBase::connectSignalsToSocket()
{
    if(connection)
        connect(connection, SIGNAL(signalGotData()), this, SLOT(slotGotData()));
}

//-------------------------------------------------------------------------------------------------
ProtocolBase::~ProtocolBase()
{
    if(connection)
        disconnect(connection, SIGNAL(signalGotData()), this, SLOT(slotGotData()));
}

//-------------------------------------------------------------------------------------------------
void ProtocolBase::send(ProtocolBasePacket *packet, int client_id)
{
	if (!connection || connection->getDescriptorID() != client_id) //не нам отправлять
		return;

    QByteArray dataSend = packet->toData();

	delete packet;

    emit signalSendByteArray(dataSend);
}

//-------------------------------------------------------------------------------------------------
void ProtocolBase::slotGotData()
{
    if(connection)
        processData(connection->readAll(), connection->getDescriptorID());
}
//-------------------------------------------------------------------------------------------------
