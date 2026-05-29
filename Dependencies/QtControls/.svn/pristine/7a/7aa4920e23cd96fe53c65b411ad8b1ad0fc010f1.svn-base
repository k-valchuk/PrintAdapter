//----------------------------------------------------------------------
//Редактор горячих клавиш
//----------------------------------------------------------------------
// ObjectName = ViewEditor (для стилей)
//--------------------------------------------------------------------------------
#ifndef HOTKEYEDITOR_H
#define HOTKEYEDITOR_H

#include <QWidget>
#include "QFocusEvent"
class QPushButton;
class QLineEdit;

class HotKeyEditor : public QWidget
{
    Q_OBJECT
public:
    explicit HotKeyEditor(QWidget *parent = 0);
    void setKey(const QString &key);            //установить ГК
    QString getKey()const;                      //получить ГК

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;     //фильтр событий
    virtual void resizeEvent(QResizeEvent *) override;                  //изменить размер

private:
    QPushButton *clearBP;   //кнопка очистки ГК
    QLineEdit *hkEdit;      //текстовое поле
signals:
    void sig_editingFinished();//завершить редактирование
};

#endif // HOTKEYEDITOR_H
