#pragma once

#include <QDialog>
#include <QComboBox>
#include <QTextBrowser>
#include "ServerRequester.h"

class QComboBox;


enum class ContentType { HTML, TEXT };

class RequestDialog: public QDialog {
    Q_OBJECT
    

    private:
        QTextBrowser* browser;
        ServerRequester* server_requester;
    
    public:
        RequestDialog(QWidget* pwgt, ServerRequester* server_requester);

    
    public slots:
        void changeBrowserContent(QString content, ContentType content_type);
};