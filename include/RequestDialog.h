#pragma once

#include <QDialog>
#include <QButtonGroup>
#include <QTextBrowser>
#include "ServerRequester.h"
#include "BaseRequestButton.h"


enum class ContentType { HTML, TEXT };

class RequestDialog: public QDialog {
    Q_OBJECT
    

    private:
        QTextBrowser* browser;
        ServerRequester* server_requester;
        QButtonGroup* button_group;

        class ContentModel {
            public:
                QString content;
                ContentType content_type;
                ContentModel(QString content, ContentType content_type) : content(content), content_type(content_type){};
                ContentModel() : content(""), content_type(ContentType::HTML){};
        };

        ContentModel responseRouting(const QJsonDocument jsonDoc, ActionId buttonId);
        void requestRouting(ActionId buttonId);
    
    public:
        RequestDialog(QWidget* pwgt, ServerRequester* server_requester);

    
    public slots:
        void changeBrowserContent(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId);
        void getErrorRequestSlot(QString message, int httpStatus);
    
    private slots:
        void handleButtonClicked(QAbstractButton *button);

};