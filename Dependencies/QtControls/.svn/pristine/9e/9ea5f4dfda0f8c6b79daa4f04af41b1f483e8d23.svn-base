#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QThread>

#include "Logger.h"

//Logger ----------------------------------------------------------------------------------------------
Logger::Logger(const QString &file_name_in, const int size_of_log_in, 
			   WriteDirection writeDirection_in, WriteType writeType_in, bool clearBeforeUsage, QObject *parent)
	   :QFile(file_name_in,parent),
	   log_file_name(file_name_in),
	   size_of_log(size_of_log_in),
	   count_buffer_message(0),
	   writeDirection(writeDirection_in),
	   writeType(writeType_in)	   
{
	last_date = QDate::currentDate();

	//чтение существующего файла
	if (size_of_log > 0 || writeType==eCycleLog)
	{
		open(QIODevice::ReadOnly);
		buffer = readAll();
		close();
	}

	if (writeType==eWriteForDate)
		setCurrentFileName(last_date);

	if (clearBeforeUsage) //очищаем перед использованием
		open(QIODevice::WriteOnly);
	else //режим дописывания
		open(QIODevice::Append);
}

//----------------------------------------------------------------------------
Logger::~Logger()
{
    if (!buffer_message.isEmpty())
        writeToFile(buffer_message);

    close();
}

//----------------------------------------------------------------------------
void Logger::cleanLogs(const QString &dir_logs, const QString &mask_logs, const int &day_ago_logs)
{
	QDir			inDirCurrent(dir_logs, 
							 mask_logs, //mask
							 QDir::Time, //sort
							 QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot); //filter

	QFileInfoList	current_files = inDirCurrent.entryInfoList();
	int	i;

	for (i=0;i<current_files.count();i++)
	{
		QFile inFinFile(current_files.at(i).absoluteFilePath());
		if (current_files.at(i).created().addDays(day_ago_logs) < QDateTime::currentDateTime())
			inFinFile.remove();
	}
}

//----------------------------------------------------------------------------
void Logger::setLogFileName(const QString &file_name)
{
	QMutexLocker locker(&m_mutex); //for thread-safe

	log_file_name = file_name;

	close(); //закрываем текущий файл

	if (writeType==eWriteForDate)
		setCurrentFileName(QDate::currentDate());
	else
		setFileName(log_file_name);

	open(QIODevice::WriteOnly); //открываем новый!
}

//----------------------------------------------------------------------------
Logger &Logger::operator << (const QString &s)
{
	writeToBuffer(s);
	return *this;
}

//----------------------------------------------------------------------------
Logger &Logger::operator << (const char *s)
{
	writeToBuffer(QString(s));
	return *this;
}

//----------------------------------------------------------------------------
Logger &Logger::operator << (double f)
{
	writeToBuffer(QString("%1").arg(f,0,'f',12));
	return *this;
}

//----------------------------------------------------------------------------
Logger &Logger::operator << (int i)
{
	writeToBuffer(QString::number(i));
	return *this;
}

//----------------------------------------------------------------------------
Logger &endl(Logger &l)
{
	l.writeToFile();
	return l;
}

//----------------------------------------------------------------------------
void Logger::SaveMessage(const QString &str, unsigned int )
{
	QDateTime cur_dt = QDateTime::currentDateTime();

	char pointerStr[64];
	sprintf(pointerStr, "%p", (int*)QThread::currentThreadId());
	const QString log_out(QString("%1 [%2] %3\n").arg(cur_dt.toString("dd.MM.yy hh:mm:ss:zzz")).arg(pointerStr).arg(str));

	if (writeType == eWriteForDate && cur_dt.date().daysTo(last_date) != 0) //режим записи один файл - один день, переход на след. день
	{
		QMutexLocker locker(&m_mutex); //for thread-safe

		close(); //закрываем текущий файл

		setCurrentFileName(cur_dt.date());
		open(QIODevice::WriteOnly); //открываем новый!
	}

	last_date = cur_dt.date(); //запоминаем новую дату

	writeToFile(log_out);
}

//----------------------------------------------------------------------------
void Logger::setCurrentFileName(const QDate &date)
{
	setFileName(log_file_name + date.toString("yyyy_MM_dd.log"));
}

//----------------------------------------------------------------------------
void Logger::writeToBuffer(const QString &str) //!< запись сообщений при использовании оператора <<
{
	buffer_message.append(str);
	count_buffer_message++;

	if (count_buffer_message > MAX_COUNT_OPERATOR_MESSAGE)
	{
        //buffer_message.append(QString::fromUtf8("\n-------Logger. Переполнение буфера при использовании оператора <<!-------\n"));
		writeToFile(buffer_message);
		count_buffer_message=0;
		buffer_message = "";
	}
}

//----------------------------------------------------------------------------
void Logger::writeToFile()
{
	buffer_message.append("\n");
	writeToFile(buffer_message);
	count_buffer_message=0;
	buffer_message = "";
}

//----------------------------------------------------------------------------
void Logger::writeToFile(const QString &log_out)
{
	QMutexLocker locker(&m_mutex); //for thread-safe

	if (isOpen())
	{
		if (writeType!=eCycleLog) //не циклический log
		{
			write(log_out.toUtf8());
		}
		else //циклический (с ограничением по размеру)
		{
			switch(writeDirection)
			{
				case eBackward:
					buffer.prepend(log_out.toUtf8());
					buffer = buffer.left(size_of_log); //ограничение по размеру
				break;

				case eForward:
					buffer.append(log_out.toUtf8());
					buffer = buffer.right(size_of_log); //ограничение по размеру
				break;
			}
			resize(0);
			write(buffer);
		}
		flush();
	}
	else
	{
        qWarning("Can't open file for write! name=%s", qPrintable(log_file_name));
	}
}
