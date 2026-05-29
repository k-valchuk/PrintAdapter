//===========================================================================================================
//		Импорт Bram player'а с использованием qtwinmigrate для использования в Qt приложениях
//	Использование ISoftPlayer через прослойку BramPlayer::Dispatcher для запуска в отдельном потоке
//===========================================================================================================
#ifndef _BRAMPLAYERWIDGET_H_
#define _BRAMPLAYERWIDGET_H_

#include <windows.h>
#include <QThread>

#include "qwinhost.h"
#include "BramPlayerDispatcher.h"

class QAction;
class QMenu;

// Импорт Bram Player через QWinHost
class BramPlayerWidget : public QWinHost
{
	Q_OBJECT
private:	
	HWND createWindow(HWND parent, HINSTANCE instance);	

    // Поток в котором будет работать диспетчер
	QThread dispatcherThread;
    // Диспетчер объекта ISoftPlayer
    BramPlayer::Dispatcher* bpDispatcher;

protected:
	// Контекстное меню плеера
	QMenu* contexMenu;
	virtual void initMenu(void) { }

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void repaint(void);

	void drawEmptyFrame(HDC dc);

	// Установка кол-ва пикметров (для диспетчера)
	// Для применения необходимо вызвать функцию reload
	void setPeekmeterCount(int count);

public:
	BramPlayerWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
	~BramPlayerWidget();

    // Показать контекстное меню в позиции курсора
	void showContexMenu(void);

	// Получения моментальных параметров
    BramPlayer::EPlayerState state(void);
	unsigned long pos(void);
	
	// Очистка плеера
	void clear(void);

	// Добавления клипа для проигрывания
	void appendClip(QString path, int32_t frameDuration = -1, int32_t framePrepare = 0);

	// Перезагрузка плеера с обновляемой длительностью
	// Перезагрузка плеера с сохранением статусов
	// При перезагрузки плеера мы выбираем нужно ли сохранять статус который был до перезагрузки или нет(установить в паузу)
	// Делаем это из-за того что в некоторых моментах происходит ошибка восстановления статуса (последоветльность seek(p)->play=seek(0)->play )
	void reload(unsigned long fileFrameDuration, bool isRecoverState = false);
	// Перезагрузка плеера с текущей(сохраненной) длительностью
	void reload(bool isRecoverState = false);
	// Установка размера плеера
    void setPlayerSize(BramPlayer::EPleerSize size);
	// Установить частоту кадров работы плеера
	void setPlayerRate(unsigned int rate, unsigned int scale);
	// Элементарные команды управления
	void pause(void);
	void play(void);
	void stop(void);

	void seek(unsigned int pos);
	void seek(int pos);

    //Звук при перемотке
    void setAudioScrub(bool bS);
    //Деинтерлейс
    void setDeinterlace(bool bS);

	// Загружены ли данные в плеере
	bool isVideoPresent(void);

private slots:
    void dispatcherProblem(const QString &msg);

signals:
	void posChanged(int pos);
    void stateChanged(BramPlayer::EPlayerState state);
	// Сигнал что файл который был добавлен через appendClip(или reload) подготовлен
	void filePrepared(bool isSuccessfull);	
};

#endif // _BRAMPLAYERWIDGET_H_
