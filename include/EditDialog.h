#pragma once

#include <QButtonGroup>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QCheckBox>
#include "BaseTableView.h"
#include "BaseDialog.h"
#include "ServerRequester.h"


class EditDialog: public BaseDialog {
    Q_OBJECT

    private:
        ServerRequester* server_requester;
        QStandardItemModel* templateRundownModel;
        QStandardItemModel* templateStoryModel;
        BaseTableView* templateRundownTableView;
        BaseTableView* templateStoryTableView;
        QButtonGroup* button_group;

        QCheckBox* activeCheckBox;
        QTextEdit* templateEdit;
        QString currentSubSystem;
    
    public:
        EditDialog(QWidget* pwgt, ServerRequester* server_requester_);
        void updateTable(QJsonArray value_array, QStandardItemModel* itemModel, bool single);
    
    public slots:
        void showResponseSlot(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId);
        void getErrorRequestSlot(QString message, int httpStatus);
};