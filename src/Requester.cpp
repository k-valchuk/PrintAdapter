#include "Requester.h"
#include <QTimer>
#include <QNetworkProxyFactory>

Requester::Requester(QObject* pobj) : QObject(pobj){
    networkManager = new QNetworkAccessManager(this);
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
}

QNetworkReply* Requester::sendRequest(QNetworkRequest request, RequestTypes requestType, const QByteArray& data) {
    if ( requestType == RequestTypes::GET) {
        return networkManager->get(request);
    } else if (requestType == RequestTypes::POST) {
        return networkManager->post(request, data);
    } else if (requestType == RequestTypes::DELETE_RESOURCE) {
        return networkManager->deleteResource(request);
    }
    
    return nullptr;
    
}

QNetworkReply* Requester::generateReply(const QUrl& url, RequestTypes requestType, const QByteArray& data){
    QNetworkRequest request(url);
    QNetworkReply* reply = sendRequest(request, requestType, data);
    QTimer* timeout = new QTimer(reply);
    timeout->setSingleShot(true);
    timeout->setInterval(20000);
    QObject::connect(timeout, &QTimer::timeout, reply, [reply] {
            if (reply->isRunning()) {
                reply->setProperty("timed_out", true);
                reply->abort();
            }
    });
    timeout->start();
    return reply;
}

void Requester::restRequest(const QUrl& url, RequestTypes requestType, const QByteArray& data){
    QNetworkReply* reply = generateReply(url, requestType, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        const bool timedOut = reply->property("timed_out").toBool();
        const int http = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (timedOut) {
            emit error(QString("Timeout"), http);
            reply->deleteLater();
            return;
        }
        const QByteArray body = reply->readAll();
        if (reply->error() != QNetworkReply::NoError) {
            emit error(reply->errorString(), http);
        } else {
            emit done(http, body);
        }
        reply->deleteLater();
    });
}