//----------------------------------------------------------------------
// Многострочный редактор в ячейке списка
//----------------------------------------------------------------------
// - перевод на новую строку по Shift+Enter
// - сменить регистр выделенного текста Shift+F3
//----------------------------------------------------------------------

#ifndef ITEMTEXTEDIT_H
#define ITEMTEXTEDIT_H

#include <QTextEdit>
#include <QPlainTextEdit>

// Получить параметры для отображения/редактирования
class IGetTextEditParams
{
public:
    // Получить максимальную длину текста
    virtual int getMaxLength(const QString &text) const = 0;
};

class ItemTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    ItemTextEdit(QWidget *parent = nullptr);

    // Задать текст
    void setText(const QString &text);

    // Задать получатель параметров
    void setTextEditParams(IGetTextEditParams *gtep) {TextEditParams = gtep;}

protected:
    void keyPressEvent(QKeyEvent *keyEvent) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void sig_textChanged(const QString &);      //изменен текст
    void sig_editingFinished();                 //редактирование завершено (по Enter)

private slots:
    void slot_textChanged();                    //текст изменен

    // Изменить регистр выделенного текста
    void slot_allUpperCase();   // Все заглавные
    void slot_allLowerCase();   // Все строчные
    void slot_asInSentences();  // Как в предложениях
private:

    QString oldText;
    IGetTextEditParams *TextEditParams;

    // Изменить регистр выделенного текста
    void setSelectedTextFontCapitalization(QFont::Capitalization fontCap);
    // Содержит ли текст строчную букву
    bool hasLowerLetter(const QString &text) const;
    // Смена регистра выделенного текста
    void changeSelectedTextFontCapitalization();
};

#endif // ITEMTEXTEDIT_H
