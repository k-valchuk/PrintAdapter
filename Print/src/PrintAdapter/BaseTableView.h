#pragma once

#include <QTableView>
#include <QStandardItemModel>

class BaseTableView: public QTableView {
    Q_OBJECT

    public:
    BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel, QColor rowColor, bool editable);
    signals:
        void deleteTemplate(QString templateId);

};