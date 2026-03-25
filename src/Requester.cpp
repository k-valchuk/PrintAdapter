#include "Requester.h"
#include <QTimer>
#include <QDebug>
#include <QNetworkProxyFactory>

Requester::Requester(QObject* pobj) : QObject(pobj){
    networkManager = new QNetworkAccessManager(this);
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
}

QNetworkReply* Requester::generateReply(const QUrl& url, const QString& keyHeaderName){
    QNetworkRequest request(url);
    QNetworkReply* reply = networkManager->get(request);
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

void Requester::getRequest(const QUrl& url, const QString& keyHeaderName){
    QNetworkReply* reply = generateReply(url, keyHeaderName);
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