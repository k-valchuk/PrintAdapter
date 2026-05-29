//---------------------------------------------------------------------
//            TCEditWidget.h
//
//		Класс для редактирования таймкода
//---------------------------------------------------------------------
// - завершение редактирования по нажатию клавиш Enter и Esc
//---------------------------------------------------------------------
#ifndef _TCEDITWIDGET_H_
#define _TCEDITWIDGET_H_

#include "TCOperations.h"

#include <QLineEdit>
#include <QTextLayout>

class TCEdit : public QLineEdit
{
	Q_OBJECT

private:
	TC currentTC, savedTC; //текущий такймкод и сохраненный(используется при арифметических действиях)
	TC prevTC;

	double frameRate;
	bool wrongKey;//ввод неверной цифры в разряд

	const int startCursorPosition, startCursorCalcPosition;	// начальные позиции курсора в разных режимах
	int numEnteredCalcSymbols; // Кол-во введенных символов при арифметических действиях,
								// для простого подсчета и разукрашивания цветом текста
	enum ECalcMode
	{
		ECMNone = 0,
		ECMPlus,
		ECMMinus
	}calcMode;

	QString moveDigitsRank(QString);	// сдвиг значений строки(используется при вводе значений при арифметике)
	void setOverCursorPosition();       //установить позицию по текущему курсору мыши
    void checkCursorPosition(int &curP);//проверить позицию на выход за рамки таймкода, изменить

protected:
    virtual void keyPressEvent(QKeyEvent * event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event)override;

public:
    // Установить/получить частоту кадров в сек
	void setFrameRate(double rate);
	inline double getFrameRate(void) const { return frameRate; }

	explicit TCEdit( QWidget * parent = NULL);

	~TCEdit();

	// Установить таймкод
	void setTC(TC tc);
	// Получить текущий таймкод (который сейчас отображается в виджете)
	inline TC getCurrentTC(void) const { return currentTC; }
	// Получить предыдущий таймкод (который был до последего изменения. Для использования в undo функциях)
	inline TC getPrevTC(void) const { return prevTC; }

	// Сброс таймкода
	void reset(void);
	void setSelectedStyleSheet(bool bSelected);//изменить стиль при выделении

	void setTCFont(QFont f);

public slots:
	void setText(const QString& str);

private slots:
	void onTextEdited(const QString&);
	void onTextChanged(const QString&);
	void onDeSelect();                   //сбросить выделение
	void onCursorPositionChanged(int, int);

signals:
	// Таймкод был изменен пользователем
	void tcEdited(TC tc);
	// Таймкод был изменен вообще (Пользователем или программно)
	void tcChanged(TC tc);
};


#endif //_TCEDITWIDGET_H_
