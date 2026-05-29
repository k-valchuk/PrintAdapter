#pragma once

//================================================================== 
//--- Logger
//--- Класс предназначен для записи отладочных сообщений в файл с возможностью
//--- использования из разных потоков
//--- Основные задачи, выполняемые классом:
//      1. Запись сообщений в файл с автоматической подстановкой текущей даты и времени
//================================================================== 

#include <QString>
#include <QFile>
#include <QMutex>
#include <QDate>

#define MAX_COUNT_OPERATOR_MESSAGE	20 //!< максимальное количество вызовов оператора <<, после которых производится запись в файл

#define ADD_FUNC_LINE(M) (QString("%1 + %2 : %3").arg(__FUNCTION__).arg(__LINE__).arg(M))
#define ADD_FUNC(M) (QString("%1: %2").arg(__FUNCTION__).arg(M))

class Logger : public QFile
{
	Q_OBJECT

public:
	enum WriteDirection //!< направление записи
	{
		eForward = 0,	//!< дописывает в конец файла
		eBackward = 1	//!< дописывает в начало файла (работает только когда size_of_log > 0)
	};

	enum WriteType	//!< тип записи в log
	{
		eCycleLog = 0,		//!< циклическая запись в один файл
		eSimpleWrite = 1,	//!< запись в один файл, без цикла (только с WriteDirection==eForward)
		eWriteForDate = 2	//!< запись в файлы по дням (каждый день - один файл, к имени добавляется дата в форомате yyyy_MM_dd)
	};

	Logger(const QString &file_name_in, int size_of_log_in = 0, 
			WriteDirection writeDirection_in = eForward, 
			WriteType writeType_in = eCycleLog,
			bool clearBeforeUsage = false, QObject *parent = 0);
	~Logger();

	void setLogFileName(const QString &file_name);
	void SaveMessage(const QString &str, unsigned int code=0); //!< запись сообщения в файл, с простановкой
	static void cleanLogs(const QString &dir_logs, const QString &mask_logs, const int &day_ago_logs);

    Logger &operator << (const QString &s); //!< запись сообщения в буфер
    Logger &operator << (const char *s);	//!< запись сообщения в буфер
	Logger &operator << (double f);			//!< запись числа double в буфер по формату arg(f,0,'f',12)
	Logger &operator << (int i);			//!< запись числа в буфер QString::number()

	void writeToFile();						//!< запись буфера в файл
    void writeToFile(const QString &log_out); //!< запись строки в файл

private:
	QMutex m_mutex; //!< для работы из нескольких потоков
	QString log_file_name; //!< имя файла
	unsigned int size_of_log; //!< max размер файла для циклической записи, если 0 - без ограничений
	QByteArray buffer; //!< буфер для циклической записи
	QString buffer_message; //!< буфер для хранения сообщений при использовании операторов << (max 20 сообщений)
	int count_buffer_message; //!< количество вызовов оператора <<
	WriteDirection writeDirection; //!< направление записи в файл
	WriteType writeType; //!< тип записи в файл
	QDate last_date; //!< дата и время последнего сообщения

	void setCurrentFileName(const QDate &date);

	void writeToBuffer(const QString &str); //!< запись сообщений в буфер при использовании оператора <<
};

//Манипулятор------------------------------------------------------------------
typedef Logger & (*LoggerFunction)(Logger &);// manipulator function
inline Logger &operator << (Logger &s, LoggerFunction f) { return (*f)(s); }
Logger &endl(Logger &l); //!< сбрасывает buffer_message в файл
//Манипулятор------------------------------------------------------------------
