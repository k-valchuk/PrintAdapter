#pragma once

#include <QTableView>
#include <QStandardItemModel>
#include "BaseDelegate.h"

class BaseTableView: public QTableView {
    Q_OBJECT

    public:
    BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel, BaseDelegate* itemDelegate);
    signals:
        void deleteTemplate(QString templateId);

};