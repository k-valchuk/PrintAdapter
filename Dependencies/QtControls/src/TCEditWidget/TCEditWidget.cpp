#include "TCEditWidget.h"
#include <QEvent>
#include <QKeyEvent>
#include <QtWidgets/QApplication>
#include <QFocusEvent>
#include <QRegExpValidator>

// Набор regexp значений доступных для ввода к частоте кадров
const QMap<double, QString> framerateValidRegexpMap = {
	{ 25, "^(\\+|\\-)?([0-9][0-9])(:[0-5][0-9]){2}(:([0-1][0-9]|2[0-4]))$" },
	{ 50, "^(\\+|\\-)?([0-9][0-9])(:[0-5][0-9]){2}(:([0-4][0-9]))$" }
};

TCEdit::TCEdit( QWidget * parent) : QLineEdit(parent), currentTC(0), savedTC(0), prevTC(0), frameRate(25),
    startCursorPosition(10), startCursorCalcPosition(11), calcMode(ECMNone)
{
    setInputMask("99:99:99:99");

    connect(this, &QLineEdit::textEdited, this, &TCEdit::onTextEdited);
	connect(this, &QLineEdit::textChanged, this, &TCEdit::onTextChanged);
	connect(this, &QLineEdit::cursorPositionChanged, this, &TCEdit::onCursorPositionChanged);
    connect(this, &QLineEdit::selectionChanged, this, &TCEdit::onDeSelect);

	setObjectName("tcedit");
	reset();
    wrongKey = false;
    installEventFilter(this);
}

TCEdit::~TCEdit()
{
}

void TCEdit::setTC(TC tc)
{    
	prevTC = tc;

	const int cursorPos = cursorPosition();

	currentTC = TCOperations::checkTC(tc, frameRate);
	QLineEdit::setText(TCOperations::getTCAsString(currentTC, frameRate));

	// устанавливаем позицию курсора
	setCursorPosition(cursorPos);
}

void TCEdit::keyPressEvent(QKeyEvent * event)
{
	switch (event->key())
	{
	case Qt::Key_0:case Qt::Key_1:case Qt::Key_2:case Qt::Key_3:case Qt::Key_4:case Qt::Key_5:
	case Qt::Key_6:case Qt::Key_7:case Qt::Key_8:case Qt::Key_9:
	{
		int cursorPos = cursorPosition();

		if (calcMode == ECMNone)
		{
            QLineEdit::keyPressEvent(event);
            if (!wrongKey)
            {
                cursorPos++;

                // для того, чтобы курсор перескакивал двоеточия
                if (cursorPos == 2 || cursorPos == 5 || cursorPos == 8)
                    cursorPos++;
            }
            else
                wrongKey = false;

            checkCursorPosition(cursorPos);
            setCursorPosition(cursorPos);
			return;
		}
		else
		{
			//смещаем все знаки строку влево
			QString str = moveDigitsRank(text());
			// записываем в последний элемент строки введенное значение
            str[str.size() - 1] = 48 + event->key() - Qt::Key_0;
			QLineEdit::setText(str);
            //устанавливаем курсор в крайнее правое положение
            setCursorPosition(startCursorPosition);
		}
	}
		break;
	case Qt::Key_Plus:
		calcMode = ECMPlus;

		savedTC = TCOperations::strToTC(text(), frameRate);

		setInputMask("+99:99:99:99");
		QLineEdit::setText("+00:00:00:00");
		
		setCursorPosition(startCursorCalcPosition);

		break;
	case Qt::Key_Minus:
		calcMode = ECMMinus;

		savedTC = TCOperations::strToTC(text(), frameRate);

		setInputMask("-99:99:99:99");
		QLineEdit::setText("-00:00:00:00");

		setCursorPosition(startCursorCalcPosition);

		break;
	case Qt::Key_Enter: case Qt::Key_Return:
		{
			// Первоначальный режим с которым мы сюда пришли
			ECalcMode initmode = calcMode;

			// Устанавливаем режим как обычный, для того чтобы textChangedSlot отправлялся сигнал
			calcMode = ECMNone;

			switch (initmode)
			{
			case ECMPlus:
				currentTC = savedTC + TCOperations::strToTC(text(), frameRate);
				QLineEdit::setText(TCOperations::getTCAsString(currentTC, frameRate));

				emit tcEdited(currentTC);
				break;
			case ECMMinus:
				currentTC = savedTC - TCOperations::strToTC(text(), frameRate);

				QLineEdit::setText(TCOperations::getTCAsString(currentTC, frameRate));

				emit tcEdited(currentTC);
				break;
            default:
                break;
			}

			setInputMask("99:99:99:99");
			setCursorPosition(startCursorPosition);
            this->editingFinished();
		}
		return;

		break;
	case Qt::Key_Escape:
		setInputMask("99:99:99:99");
		setCursorPosition(startCursorPosition);

		calcMode = ECMNone;
        this->editingFinished();
        return;
	case Qt::Key_Backspace:
		return;
     case Qt::Key_Right://не уходить за границы таймкода
        if (cursorPosition() >= text().length() - 1)
            return;
	}
    QLineEdit::keyPressEvent(event);
}

void TCEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    repaint();//перерисовка для применения выравнивания текста
    setOverCursorPosition();//устанавливаем позицию курсора
}

bool TCEdit::eventFilter(QObject *, QEvent *event)
{
    //перехват событий нажатия мыши вне таймкода
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonDblClick)
    {
        int cursorPos = cursorPositionAt(mapFromGlobal(cursor().pos()));
        if (cursorPos > text().length() - 1)
        {
            return true;
        }
    }

    return QLineEdit::event(event);
}


/*
    16 — стандартная частота съёмки и проекции немого кинематографа;
    18 — стандартная частота съёмки и проекции любительского формата «8 Супер»;
    23,976 (24×1000÷1001) — частота телекинопроекции в американском стандарте разложения 525/60, применяемая для интерполяции без потерь;
    24 — общемировой стандарт частоты киносъёмки и проекции;
    25 — частота киносъёмки, применяемая при производстве телефильмов и телерепортажей для перевода в европейский стандарт разложения 625/50. Также использовалась в советской панорамной киносистеме «Кинопанорама»;
    26 — частота съёмки и проекции панорамной киносистемы «Синерама»[10];
    29.97002616 (30×1000÷1001) — точная кадровая частота цветного телевизионного стандарта NTSC;
    30 — частота киносъёмки и проекции раннего варианта широкоформатной киносистемы «Тодд-AO»;
    48 — частота съёмки и проекции кинематографических систем «IMAX HD» и «Maxivision 48»;
    50 — частота полукадров европейского стандарта разложения. Используется в электронных камерах для ТВЧ;
    59,94 (60×1000÷1001) — точная полукадровая частота цветного телевизионного стандарта NTSC и частота кадров некоторых стандартов ТВЧ;
    60 — частота киносъёмки в американском стандарте ТВЧ и системе «Шоускан» (англ. Showscan)[11].
*/
void TCEdit::setFrameRate(double rate)
{
	QList<double> frameRates({16, 18, 24 * 1000 % 1001, 24, 25, 26, 30 * 1000 % 1001, 30, 48, 50, 60 * 1000 % 1001, 60});
	// Проверка на некорректные значения частот кадров
	if (frameRates.contains(rate))
		frameRate = rate;
	else
	{
		// Некорректная частота кадров
		Q_ASSERT(frameRate);

		frameRate = 25;
	}

	setTC(currentTC);
}

void TCEdit::setText(const QString& str)
{
	setTC(TCOperations::strToTC(str, frameRate));
}

void TCEdit::onTextEdited(const QString &text)
{
	TC lTC = 0;

	QRegExp rx(framerateValidRegexpMap[frameRate]);
    if ((lTC = TCOperations::strToTC(text, frameRate)) != currentTC)
	{
        if (!rx.exactMatch(text) && calcMode == ECMNone)
        {
			QLineEdit::setText(TCOperations::getTCAsString(currentTC, frameRate));
            wrongKey = true;
            return;
        }
		currentTC = lTC;
		// Если не включен режим калкуляции, только тогда сообщаем о изменении
		// т.е. в режиме калькуляции не сообщаем о измении если был изменен текст
		if (calcMode == ECMNone)
			emit tcEdited(getCurrentTC());
	}
}

void TCEdit::reset(void)
{
	QLineEdit::setText("00:00:00:00");
	currentTC = 0;
	savedTC = 0;
	calcMode = ECMNone;

	setCursorPosition(startCursorPosition);
    update();
}

void TCEdit::setSelectedStyleSheet(bool bSelected)
{
    if (bSelected)
        setStyleSheet("background-color: rgba(85, 170, 255, 10);");
    else
        setStyleSheet("");

}

void TCEdit::setTCFont(QFont f)
{
    f.setFamily(f.family());
    f.setPointSize(f.pointSize());
    setFont(f);
}

void TCEdit::onTextChanged(const QString &)
{

	// Если не включен режим калкуляции, только тогда сообщаем о изменении
	// т.е. в режиме калькуляции не сообщаем о измении если был изменен текст
	if(calcMode == ECMNone)
        emit tcChanged(getCurrentTC());
}

void TCEdit::onDeSelect()
{
    this->deselect();
}

void TCEdit::onCursorPositionChanged(int , int )
{
}

QString TCEdit::moveDigitsRank(QString str)
{
	for (int i = 1; i < str.size() - 1; i++)
	{
		if (str[i + 1] != '+' && str[i + 1] != '-' && str[i + 1] != ':')
		{
			if ((str[i] == '+' || str[i] == '-' || str[i] == ':') && i > 0)
				str[i - 1] = str[i + 1];
			else
				str[i] = str[i + 1];
		}
	}
    return str;
}

void TCEdit::setOverCursorPosition()
{
    int cursorPos = cursorPositionAt(mapFromGlobal(cursor().pos()));
    checkCursorPosition(cursorPos);
    setCursorPosition(cursorPos);
}

void TCEdit::checkCursorPosition(int &curP)
{
    if (curP > text().length() - 1)
        curP = text().length() - 1;
}
