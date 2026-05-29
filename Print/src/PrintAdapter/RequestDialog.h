#pragma once

#include <QStandardItemModel>
#include "BaseTableView.h"
#include "BaseDialog.h"
#include "ServerRequester.h"


enum class ContentType { HTML, TEXT, EMPTY };

class RequestDialog: public BaseDialog {
    Q_OBJECT
    

    private:
        QStandardItemModel* templatesModel;
        BaseTableView* templateTableView;
        ServerRequester* server_requester;
        QString subsystemId;

        class ContentModel {
            public:
                QString content;
                ContentType content_type;
                QString title;
                ContentModel(QString content, ContentType content_type, QString title) : content(content), content_type(content_type), title(title){};
                ContentModel() : content(""), content_type(ContentType::HTML), title(""){};
        };

        ContentModel responseRouting(const QJsonDocument jsonDoc, ActionId buttonId);
        void updateTable(QJsonArray value_array, QStandardItemModel* itemModel);
        void removeRowByID(int itemID, QStandardItemModel* itemModel);
       
    
    public:
        RequestDialog(QWidget* pwgt, QJsonDocument json_doc, QString subsystemId);

    
    public slots:
        void showResponseSlot(const QJsonDocument jsonDoc, ActionId buttonId);
        void getErrorRequestSlot(QString message, int httpStatus);
        void printTemplate();
};