#include "itemTextEdit.h"
#include "QMenu"
#include "QKeyEvent"

// только буквы
const char* OnlyLetters("[^\\W\\d_]");

const QColor MaxLengthValueBGColor("#66E67714");  // Цвет фона текста превышения длины (колонка значение)
const QColor MaxLengthValueTColor("#be926a");     // Цвет текста превышения длины (колонка значение)

ItemTextEdit::ItemTextEdit(QWidget *parent) : QTextEdit(parent), TextEditParams(nullptr)
{
    setAcceptRichText(false);
    document()->setDocumentMargin(1);
    connect(this, SIGNAL(textChanged()), SLOT(slot_textChanged()));
}

void ItemTextEdit::setText(const QString &text)
{
    //задать текст
    setPlainText(text);
    //перевод курсора в конец строки
    QTextCursor cur = textCursor();
    cur.movePosition(QTextCursor::EndOfLine);
    setTextCursor(cur);
}

void ItemTextEdit::changeSelectedTextFontCapitalization()
{
//    Выделив текст и нажав кнопку(hotkey) пользователь сможет произвести инверсию текста по следующим правилам:
//    1. Все выделенные буквы/слова заглавные (ПРИВЕТ)
//    2. Все выделенные буквы/слова строчные (привет)
//    3. В зависимости от выделения:
//    3.1 Слово : первая буква заглавная (Привет)
//    3.2 Предложение : первая буква в предложении заглавная, а остальные все строчные (Сюжет был готов к эфиру.)
//    3.3 Текст с несколькими предложениями : Первая буква первого слова в выделении, Первая буква после знаков заканчивающих предложение(. ! ?) должны быть
//    заглавными (вне зависимости через сколько символов они будут), а все остальные строчные (Добрый вечер, уважаемая публика! Вы готовы?).
//    4. После выполнения 3-его правила и если пользователь еще раз нажмет на кнопку hotkey, то он возвращается к первому правилу.

//    Предварительная проверка:
//    Изначально выделив текст должна произойти проверка:
//    1)Встречается ли в выделенном тексте хотя бы одна строчная буква?
//    1.1 Да. используем правило 1.
//    1.2 Нет. используем правило 2.
//    2)Встречается ли в выделенном тексте хотя бы одна заглавная буква?
//    2.1 Да. Она в начале выделения?
//    2.1.1. Да. используем правило 1.
//    2.1.1. Нет. используем правило 3.
//    2.2 Нет. используем правило 3.

    QTextCursor selectCursor(textCursor());
    const QString &text = selectCursor.selectedText();
    if (!text.isEmpty())
    {
        int pos = text.indexOf(QRegExp(OnlyLetters), 0);
        if (pos != -1)
        {
            if (text.at(pos).isLower())
                setSelectedTextFontCapitalization(QFont::Capitalize);
            else if (hasLowerLetter(text))
                setSelectedTextFontCapitalization(QFont::AllUppercase);
            else
                setSelectedTextFontCapitalization(QFont::AllLowercase);

        }
    }
}

void ItemTextEdit::keyPressEvent(QKeyEvent *keyEvent)
{//перевод на новую строку по Shift+Enter
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
    {
        if (keyEvent->modifiers().testFlag(Qt::ShiftModifier))
        {
            keyEvent->setModifiers(keyEvent->modifiers() &~Qt::ShiftModifier);
        }
        else
        {
            emit sig_editingFinished();
            return;
        }
    }
    else if (keyEvent->key() == Qt::Key_F3 && keyEvent->modifiers().testFlag(Qt::ShiftModifier))
    {// сменить регистр выделенного текста Shift+F3
        changeSelectedTextFontCapitalization();
    }

    QTextEdit::keyPressEvent(keyEvent);
}

void ItemTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->addSeparator();
    menu->addAction(tr("ALL UPPERCASE"), this, SLOT(slot_allUpperCase()));
    menu->addAction(tr("all lowercase"), this, SLOT(slot_allLowerCase()));
    menu->addAction(tr("As in sentences."), this, SLOT(slot_asInSentences()));

    menu->exec(event->globalPos());
    delete menu;
}

void ItemTextEdit::slot_textChanged()
{//изменен текст
    QString text = toPlainText();
    if (oldText != text)
    {
        oldText = text;
        if (TextEditParams)
        {
            int maxLength = TextEditParams->getMaxLength(text);
            if (maxLength > 0)
            {//выделяем цветом символы номера которых превышают максимальный

                QTextCursor newCursor(textCursor());
                QTextCharFormat textFormat (newCursor.charFormat());
                bool bMaxLength = text.length() > maxLength;// есть превышение

                // текста без превышения
                textFormat.setForeground(QBrush());
                textFormat.setBackground(QBrush());
                newCursor.setPosition(0, QTextCursor::MoveAnchor);
                newCursor.setPosition(bMaxLength ? maxLength : text.length(), QTextCursor::KeepAnchor);
                newCursor.setCharFormat(textFormat);

                if(bMaxLength)
                {// выделить превышение
                    textFormat.setBackground(MaxLengthValueBGColor);
                    textFormat.setForeground(MaxLengthValueTColor);
                    newCursor.setPosition(maxLength, QTextCursor::MoveAnchor);
                    newCursor.setPosition(text.length(), QTextCursor::KeepAnchor);
                    newCursor.setCharFormat(textFormat);
                }
            }
        }

        emit sig_textChanged(text);
    }
}

void ItemTextEdit::slot_allUpperCase()
{
    setSelectedTextFontCapitalization(QFont::AllUppercase);
}

void ItemTextEdit::slot_allLowerCase()
{
    setSelectedTextFontCapitalization(QFont::AllLowercase);
}

void ItemTextEdit::slot_asInSentences()
{
    setSelectedTextFontCapitalization(QFont::Capitalize);
}

void ItemTextEdit::setSelectedTextFontCapitalization(QFont::Capitalization fontCap)
{
    QTextCursor selectCursor(textCursor());

    if (selectCursor.selectedText().isEmpty())
        return;

    int startPos = selectCursor.selectionStart();
    int endPos = selectCursor.selectionEnd();
    QString selectedText = selectCursor.selectedText();

    if (fontCap == QFont::AllUppercase)// все заглавные
        selectedText = selectedText.toUpper();
    else if (fontCap == QFont::AllLowercase)// все строчные
        selectedText = selectedText.toLower();
    else if (fontCap == QFont::Capitalize)// как в предложении
    {
        selectedText = selectedText.toLower();// все строчные

        // первая буква в предложении - заглавная
        int posEndSent = 0;
        while (posEndSent != -1)
        {
            posEndSent = selectedText.indexOf(QRegExp(OnlyLetters), posEndSent);
            if (posEndSent != -1)
                selectedText.replace(posEndSent, 1, selectedText.at(posEndSent).toUpper());

            posEndSent = selectedText.indexOf(QRegExp("[.?!]"), posEndSent);
        }
    }

    selectCursor.insertText(selectedText);// замена текста

    // восстановить выделение
    selectCursor.setPosition(startPos);
    selectCursor.setPosition(endPos, QTextCursor::KeepAnchor);
    setTextCursor(selectCursor);
}

bool ItemTextEdit::hasLowerLetter(const QString &text) const
{
    int posLower = 0;
    while (true)
    {
        posLower = text.indexOf(QRegExp(OnlyLetters), posLower);
        if (posLower == -1)
            break;

        if (text.at(posLower).isLower())
            return true;

        ++posLower;
    }
    return false;
}
