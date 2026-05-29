#include "BramPlayerDispatcher.h"

#include <QtWidgets/QFileDialog>
#include <QTimerEvent>

#include "BramPlayer.h"

// Тип файла по контейнеру
enum EFileType
{
	eFTUnknown, eFTAVI, eFTMOV, eFTMXF, eFTP2MXF, eFTTD, eFTMP4
};

//------------------------------------------------------------------------------------------------
EFileType getFileType(const QString& filename)
{
	// ..ищем его. Вырезаем путь без расширения
	int indexLastPoint = filename.lastIndexOf('.');
	if (indexLastPoint != -1)
	{
		QString extension = filename.mid(indexLastPoint);
		if (extension.compare(".avi", Qt::CaseInsensitive) == 0)
			return eFTAVI;
		else if (extension.compare(".mov", Qt::CaseInsensitive) == 0)
			return eFTMOV;
		else if (extension.compare(".mxf", Qt::CaseInsensitive) == 0)
			return eFTMXF;
		else if (extension.compare(".p2mxf", Qt::CaseInsensitive) == 0)
			return eFTP2MXF;
		else if (extension.compare(".tdv", Qt::CaseInsensitive) == 0)
			return eFTTD;
		else if (extension.compare(".mp4", Qt::CaseInsensitive) == 0)
			return eFTMP4;
	}

	return eFTUnknown;
}

//------------------------------------------------------------------------------------------------
BramPlayer::Dispatcher::Dispatcher(HWND hw, QObject* parent) : QObject(parent), framePrepare(0), frameDuration(0), numPeekmeters(0),
    bramPlayerLibrary(nullptr), dllInitFunc(nullptr), dllReleaseFunc(nullptr), softPlayer2(nullptr), statePlayerTimerID(0), hwnd(hw),
    bAudioScrub(true), bShowGraphics(false), bShowSubtitles(false), bDeinterlace(false)
{
	lastState.dwPos = 0;
	lastState.dwState = cSoftPlayerStateStopped;    
}

//------------------------------------------------------------------------------------------------
BramPlayer::Dispatcher::~Dispatcher()
{
	releaseLibraryAndDestroy();
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::init(void)
{
	if (loadLibraryAndCreate())
		softPlayer2->SetWindow(hwnd);
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::isVideoPresent(void)
{
	if (!softPlayer2)
		return false;

	return softPlayer2->IsVideoPresent();
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::loadLibraryAndCreate(void)
{
	if (!bramPlayerLibrary)
		bramPlayerLibrary = new QLibrary;

	if (!bramPlayerLibrary->isLoaded())
	{
		bramPlayerLibrary->setFileName("BramPlayer.dll");
        if (!bramPlayerLibrary->load())
        {
            emit problem(bramPlayerLibrary->errorString());

            return false;
        }

		dllInitFunc = (DllInit)bramPlayerLibrary->resolve("DX_DllInit");
		dllReleaseFunc = (DllRelease)bramPlayerLibrary->resolve("DX_DllClean");
        dxCreatePlayer2 = (DxCreatePlayer2)bramPlayerLibrary->resolve("DX_CreatePlayer2");
		dxDestroyPlayer = (DxDestroyPlayer)bramPlayerLibrary->resolve("DX_DestroyPlayer");

        if (!dllInitFunc || !dllReleaseFunc || !dxDestroyPlayer || !dxCreatePlayer2)
        {
            emit problem(tr("Can't open resolve BramPlayer.dll functions!"));

            return false;
        }

		dllInitFunc();

        if (dxCreatePlayer2(&softPlayer2, BRAM_PLAYER_VERSION) == 0)
        {
            emit problem(tr("Can't create soft player!"));

            return false;
        }
        else
		{
			startFeedbackTimer();

			if (!softPlayer2->SetPlayerMode(RenderWindowless))
				return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::releaseLibraryAndDestroy(void)
{
	stopFeedbackTimer();

	if (softPlayer2)
		dxDestroyPlayer(softPlayer2);

	softPlayer2 = nullptr;

	if (dllReleaseFunc)
		dllReleaseFunc();

	if (bramPlayerLibrary && bramPlayerLibrary->isLoaded())
		bramPlayerLibrary->unload();

	delete bramPlayerLibrary;
	bramPlayerLibrary = nullptr;
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::setPlayerRate(unsigned int rate, unsigned int scale)
{
	if (!softPlayer2)
		return false;

	return softPlayer2->SetPlayerRate(rate, scale);
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::setPlayerSize(BramPlayer::EPleerSize size)
{
	if (!softPlayer2)
		return;

	switch (size)
	{
	case EPleerSize::ePSLowRes:
		softPlayer2->SetPlayerResolutions(360, 288, 360, 288, VideoAspect16_9, true);
		break;
	case EPleerSize::ePSSD:
		softPlayer2->SetPlayerResolutions(720, 576, 720, 576, VideoAspect16_9, true);
		break;
	case EPleerSize::ePSFHD:
		softPlayer2->SetPlayerResolutions(1920, 1080, 1920, 1080, VideoAspect16_9, true);

		break;
	}
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::setIsKeepAspectRatioFrame(bool is)
{
	if (softPlayer2)
		softPlayer2->KeepAspect(is);
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::appendClip(QString path, int32_t duration, int32_t prepare)
{
    // Сохраняем пути - необходимы при переподготовке плеера
	videoPath = path;
	audioPath = parepareAudioFilePath(videoPath);
	frameDuration = duration;
	framePrepare = prepare;

	return appendClip(videoPath, audioPath, frameDuration, framePrepare);
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::clear(void)
{
	if (softPlayer2)
		softPlayer2->Clean();

	videoPath.clear();
	audioPath.clear();
	framePrepare = 0;
	frameDuration = 0;

	lastState.dwPos = 0;
	lastState.dwState = cSoftPlayerStateStopped;
		
	::InvalidateRect(hwnd, 0, TRUE);
	::ShowWindow(hwnd, TRUE);
	
	emit cleared();
}

//------------------------------------------------------------------------------------------------
unsigned long BramPlayer::Dispatcher::private_pos(void)
{
	if (!softPlayer2)
		return 0;
	else
	{
		SSoftPlayerState state;

		if (!softPlayer2->GetState(&state))
			return 0;
		else
			return state.dwState;
	}

	return 0;
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == statePlayerTimerID && softPlayer2)
	{
		SSoftPlayerState s;
		if (softPlayer2->GetState(&s))
		{
			if (s.dwPos != lastState.dwPos)
				emit posChanged(s.dwPos);

			if (s.dwState != lastState.dwState)
				emit stateChanged(getPlayerState(s.dwState));

			lastState = s;
		}
	}
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::stopFeedbackTimer(void)
{
	if (statePlayerTimerID)
		killTimer(statePlayerTimerID);
	statePlayerTimerID = 0;
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::startFeedbackTimer(void)
{
	if (!statePlayerTimerID)
		statePlayerTimerID = startTimer(100);
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::reload(bool isRecoverState)
{
	if (!softPlayer2 && !videoPath.isEmpty())
		return false;

	// Сохраняем текущее положение плеера
	const SSoftPlayerState state = lastState;

	bool isSuccess = appendClip(videoPath, audioPath, frameDuration, framePrepare);
	if (isSuccess)
	{
		if (state.dwPos > 0)
			isSuccess = seek((unsigned int)state.dwPos);
		
		// Восстанавливаем статус воспроизведения
		if (isSuccess && state.dwState == cSoftPlayerStatePlaying)
		{
			// Делаем это из-за того что в некоторых моментах происходит ошибка восстановления статуса (последоветльность seek(p)->play=seek(0)->play )
			Sleep(70);
			isSuccess = play();
		}
	}

	return isSuccess;
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::reload(unsigned long frameDurFile, bool isRecoverState)
{
	// Обновляем длительность файла
	frameDuration = frameDurFile;

	return reload(isRecoverState);
}

//------------------------------------------------------------------------------------------------
BramPlayer::EPlayerState BramPlayer::Dispatcher::private_state(void)
{
	if (!softPlayer2)
		return EPlayerState::ePSStopped;
	else
	{
		SSoftPlayerState state;

		if (!softPlayer2->GetState(&state))
			return EPlayerState::ePSStopped;
		else
			return getPlayerState(state.dwState);
	}
	return EPlayerState::ePSStopped;
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::appendClip(const QString& vPath, const QString& aPath, int32_t frameDuration, int32_t framePrepare)
{
	if (!softPlayer2)
	{
		emit filePrepared(false);	// Сообщаем что подготовка закончена неудачно

		return false;
	}

	if (!softPlayer2->Clean())
	{
		emit filePrepared(false);	// Сообщаем что подготовка закончена неудачно

		return false;
	}

	const int size = 256;
	wchar_t wVideoPath[size] = { 0 };
	vPath.toWCharArray(wVideoPath);

	// Добавляем видео в плеер
	if (!softPlayer2->Append(APTIMELINE_TRACK_VIDEO, wVideoPath, 0/*framePrepare*/, framePrepare + frameDuration, 0, -1, ""))
	{
		emit filePrepared(false);	// Сообщаем что подготовка закончена неудачно

		return false;
	}

	// Если присутствует аудио файл - загружаем
	if (!aPath.isEmpty())
	{
		wchar_t wAutioPath[size] = { 0 };
		aPath.toWCharArray(wAutioPath);
		softPlayer2->Append(APTIMELINE_TRACK_AUDIO, wAutioPath, 0, frameDuration, 0, -1, "");
	}

	softPlayer2->SetPeakMeterNChannels(numPeekmeters);
    //звук при перемотке
    softPlayer2->EnableAudioScrub(bAudioScrub);
    //деинтерлейс
    softPlayer2->DoDeinterlace(bDeinterlace);

	// Подготавливаем
	if (!softPlayer2->Prepare(framePrepare))
	{
		emit filePrepared(false);	// Сообщаем что подготовка закончена неудачно

		return false;
	}

	emit filePrepared(true);	// Сообщаем что подготовка закончена удачно

	return true;
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::play(void)
{
	if (softPlayer2)
		return softPlayer2->Play();

	return false;
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::pause(void)
{
	if (softPlayer2)
		return softPlayer2->Pause();

	return false;
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::stop(void)
{
	if (softPlayer2)
		return softPlayer2->Stop();

	return false;
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::seek(unsigned int pos)
{
	if (softPlayer2)
		return softPlayer2->Seek(pos);

	return false;
}

//------------------------------------------------------------------------------------------------
bool BramPlayer::Dispatcher::seek(int pos)
{
	return seek((unsigned int)pos);
}

//------------------------------------------------------------------------------------------------
void BramPlayer::Dispatcher::repaint(void)
{
	if (softPlayer2)
	{
		softPlayer2->SetWindow(hwnd);

		PAINTSTRUCT paint;
		if (HDC dc = ::BeginPaint(hwnd, &paint))
			softPlayer2->RepaintVideo(dc);
		::EndPaint(hwnd, &paint);
	}
}

//------------------------------------------------------------------------------------------------
QString BramPlayer::Dispatcher::parepareAudioFilePath(const QString& videoFileName)
{
	// Определяем тип файла по расширению
	const EFileType fileType = getFileType(videoFileName);

	QString audioFilePath;

	// Если мы понимаем с каким файлом работаем
	if (fileType != eFTUnknown)
	{
		// ..ищем его. Вырезаем путь без расширения
		int indexLastPoint = videoFileName.lastIndexOf('.');
		if (indexLastPoint != -1)
		{
			audioFilePath = videoFileName.mid(0, indexLastPoint);

			bool isSeparateAudioFile = false;
			// Добавляем расширение по типу файла
			switch (fileType)
			{
			case eFTAVI: case eFTP2MXF:/*?*/
				isSeparateAudioFile = true;
				audioFilePath.append(".wav");
				break;
			case eFTMOV: case eFTMXF: case eFTMP4:
				audioFilePath = videoFileName;
				break;
			case eFTTD:
				isSeparateAudioFile = true;
				audioFilePath.append(".tda");
				break;
            case eFTUnknown:
                break;
			}
			// Проверяем на присутствие только в случае формирования имени файла
			if (isSeparateAudioFile)
			{
				QFileInfo check_file(audioFilePath);
				if (!check_file.exists() || !check_file.isFile())
					audioFilePath.clear();
			}
		}
	}

	return audioFilePath;
}

//------------------------------------------------------------------------------------------------
unsigned long BramPlayer::Dispatcher::pos(void)
{
	if(!softPlayer2)
		return 0;

	return lastState.dwPos;
}

//------------------------------------------------------------------------------------------------
BramPlayer::EPlayerState BramPlayer::Dispatcher::state(void)
{
	return getPlayerState(lastState.dwState);
}

//------------------------------------------------------------------------------------------------
BramPlayer::EPlayerState BramPlayer::Dispatcher::getPlayerState(unsigned int state)
{
	switch (state)
	{
	case cSoftPlayerStateShuttle: 
	case cSoftPlayerStatePaused:					
		return EPlayerState::ePSPaused;

	case cSoftPlayerStatePlaying:												
		return EPlayerState::ePSPlaying;

	case cSoftPlayerStateCompleted: 
	case cSoftPlayerStateStopped:
	default: 
			return EPlayerState::ePSStopped;
	}

	return EPlayerState::ePSStopped;
}
//------------------------------------------------------------------------------------------------
