#pragma once

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class QNetworkAccessManager;
class QNetworkReply;


enum class RequestTypes {GET, POST, DELETE_RESOURCE};

class Requester : public QObject {
    Q_OBJECT

    private:

        QNetworkAccessManager* networkManager;

        QNetworkReply* generateReply(const QUrl& url, RequestTypes requestType, const QByteArray& data);
        QNetworkReply* sendRequest(QNetworkRequest request, RequestTypes requestType, const QByteArray& data); 

    public:


        Requester(QObject* pobj = nullptr);
        void restRequest(const QUrl& url, RequestTypes requestType, const QByteArray& data);

    signals:
        void done(int httpStatus, QByteArray body);
        void error(QString message, int httpStatus);
};