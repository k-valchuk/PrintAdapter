#ifndef INDIVIDUALUSERVIEWSFORM_H
#define INDIVIDUALUSERVIEWSFORM_H

#include <QFrame>
#include "SharedDataVM.h"
#include "SharedDataTableModel.h"

namespace Ui {
class IndividualUserDataForm;
}

class IndividualUserDataForm : public QFrame, public ISharedDataForm
{
    Q_OBJECT
    
    SharedDataTableModel* m_model = nullptr;
    SharedDataVM* m_vm = nullptr;
public:
    explicit IndividualUserDataForm(SharedDataVM* vm, QWidget *parent = nullptr);
    ~IndividualUserDataForm();
    
    void EditItemName(const QModelIndex& index, QString name) override;
protected:
    virtual void FillNewItem(SharedData& item) {
        
    }
    virtual void FillExistingItem(SharedData& item) {}
    
private:
    Ui::IndividualUserDataForm *ui;
    
public slots:
    void onItemsAdded(quint64 afterId, QList<SharedData> items);
    void onItemsRemoved(QList<quint64> ids);
    void onListCleared();
    void onItemModified(SharedData item);
    
signals:
    
};

#endif // INDIVIDUALUSERVIEWSFORM_H
