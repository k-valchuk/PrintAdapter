#pragma once

#include <QButtonGroup>
#include <QTableWidget>
#include "BaseDialog.h"
#include "ServerRequester.h"
#include "BaseRequestButton.h"


enum class ContentType { HTML, TEXT, EMPTY };

class RequestDialog: public BaseDialog {
    Q_OBJECT
    

    private:
        QTableWidget* table;
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
        void updateTable(QJsonArray value_array);
    
    public:
        RequestDialog(QWidget* pwgt, ServerRequester* server_requester);

    
    public slots:
        void showResponseSlot(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId);
        void getErrorRequestSlot(QString message, int httpStatus);
        void onCellClicked(int row, int column);
    
    private slots:
        void handleButtonClicked(QAbstractButton *button);

};