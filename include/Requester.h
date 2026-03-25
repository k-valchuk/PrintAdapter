#pragma once

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class QNetworkAccessManager;
class QNetworkReply;

class Requester : public QObject {
    Q_OBJECT

    private:

        QNetworkAccessManager* networkManager;

        QNetworkReply* generateReply(const QUrl& url, const QString& keyHeaderName);

    public:

        Requester(QObject* pobj = nullptr);
        void getRequest(const QUrl& url, const QString& keyHeaderName);

    signals:
        void done(int httpStatus, QByteArray body);
        void error(QString message, int httpStatus);
};