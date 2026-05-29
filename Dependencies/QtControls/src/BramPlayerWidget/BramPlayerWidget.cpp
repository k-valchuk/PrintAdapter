#include "BramPlayerWidget.h"

#include <QtGui/QPainter>
#include <QMenu>
#include <QResizeEvent>
#include <QMessageBox>

//------------------------------------------------------------------------------------------------------------------------------
BramPlayerWidget::BramPlayerWidget(QWidget *parent, Qt::WindowFlags f) : QWinHost(parent, f),  contexMenu(nullptr),	bpDispatcher(nullptr)
{
	qRegisterMetaType<int32_t>("int32_t");
    qRegisterMetaType<BramPlayer::EPlayerState>("BramPlayer::EPlayerState");
    qRegisterMetaType<BramPlayer::EPleerSize>("BramPlayer::EPleerSize");
		
	HWND hWnd = createWindow((HWND)winId(), GetModuleHandle(0));
	setWindow(hWnd);

    bpDispatcher = new BramPlayer::Dispatcher(hWnd);
	bpDispatcher->moveToThread(&dispatcherThread);

	// При старте потока - запустить функцию инициализации
    Q_UNUSED(QObject::connect(&dispatcherThread, &QThread::started, bpDispatcher, &BramPlayer::Dispatcher::init));

	dispatcherThread.start();
    Q_UNUSED(connect(&dispatcherThread, &QThread::finished, bpDispatcher, &BramPlayer::Dispatcher::deleteLater));

    Q_UNUSED(connect(bpDispatcher, &BramPlayer::Dispatcher::posChanged, [=](int pos) {
		emit posChanged(pos);
	}));

    Q_UNUSED(connect(bpDispatcher, &BramPlayer::Dispatcher::stateChanged, [=](BramPlayer::EPlayerState state) {
		emit stateChanged(state);
	}));

	Q_UNUSED(connect(bpDispatcher, &BramPlayer::Dispatcher::filePrepared, [=](bool isSuccessfull) {
		emit filePrepared(isSuccessfull);
		}));

    // Выводим проблемы в MessageBox
    Q_UNUSED(connect(bpDispatcher, &BramPlayer::Dispatcher::problem, this, &BramPlayerWidget::dispatcherProblem, Qt::QueuedConnection));
	Q_UNUSED(connect(bpDispatcher, &BramPlayer::Dispatcher::cleared, [this]() {	repaint();	}))

	setMinimumSize(160, 90);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

//------------------------------------------------------------------------------------------------------------------------------
BramPlayerWidget::~BramPlayerWidget()
{
	dispatcherThread.quit();
	dispatcherThread.wait();
}

//------------------------------------------------------------------------------------------------------------------------------
HWND BramPlayerWidget::createWindow(HWND parent, HINSTANCE instance)
{
	static ATOM windowClass = 0;
	if (!windowClass) {
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = (WNDPROC)WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = instance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = L"bramplayerimport";
		wcex.hIconSm = NULL;

		windowClass = RegisterClassEx(&wcex);
	}

	HWND hwnd = CreateWindow((TCHAR*)windowClass, 0, WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, parent, NULL, instance, NULL);

	return hwnd;
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::setPlayerSize(BramPlayer::EPleerSize size)
{
	if(bpDispatcher)
        QMetaObject::invokeMethod(bpDispatcher, "setPlayerSize", Qt::QueuedConnection, Q_ARG(BramPlayer::EPleerSize, size));
}

//--------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::setPlayerRate(unsigned int rate, unsigned int scale)
{
	if (bpDispatcher)
		QMetaObject::invokeMethod(bpDispatcher, "setPlayerRate", Qt::QueuedConnection, Q_ARG(unsigned int, rate), Q_ARG(unsigned int, scale));
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::dispatcherProblem(const QString &msg)
{
    QMessageBox(QMessageBox::Critical, tr("Error"), msg).exec();
}

//------------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK BramPlayerWidget::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	QWidget *widget = QWidget::find((WId)GetParent(hWnd));
	BramPlayerWidget *window = qobject_cast<BramPlayerWidget*>(widget);

	if (window) {
		switch (message) {
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
			window->showContexMenu();
			break;
		case WM_PAINT:
			window->repaint();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::drawEmptyFrame(HDC dc)
{
    HBRUSH frameBrush = ::CreateSolidBrush(0);
    RECT r;

    GetClientRect(window(), &r);
    QString noVideoStr = tr("No video");

    wchar_t novideo[256] = { 0 };
    noVideoStr.toWCharArray(novideo);

    ::FillRect(dc, &r, frameBrush);
    ::SetBkColor(dc, TRANSPARENT);
    ::SetTextColor(dc, RGB(255, 255, 255)); //0x0000FF00 - green
    ::DrawText(dc, novideo, noVideoStr.size(), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    ::DeleteObject(frameBrush);
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::repaint(void)
{
	if (!bpDispatcher)
		return;
	else
	{
		if (!bpDispatcher->isVideoPresent())
		{
			PAINTSTRUCT paint;
			if (HDC dc = ::BeginPaint(window(), &paint))
				drawEmptyFrame(dc);
			::EndPaint(window(), &paint);
		}
		else
			QMetaObject::invokeMethod(bpDispatcher, "repaint", Qt::QueuedConnection);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::appendClip(QString path, int32_t frameDuration, int32_t framePrepare)
{
	if (bpDispatcher)
		QMetaObject::invokeMethod(bpDispatcher, "appendClip", Qt::QueuedConnection, Q_ARG(QString, path), Q_ARG(int32_t, frameDuration), Q_ARG(int32_t, framePrepare));
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::clear(void)
{
	if (bpDispatcher)
		QMetaObject::invokeMethod(bpDispatcher, "clear", Qt::QueuedConnection);
}

//------------------------------------------------------------------------------------------------------------------------------
unsigned long BramPlayerWidget::pos(void)
{
	if (!bpDispatcher)
		return 0;

	return bpDispatcher->pos();
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::showContexMenu(void)
{
    if(contexMenu)
        contexMenu->exec(QCursor::pos());
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::reload(unsigned long fileFrameDuration, bool isRecoverState)
{
	if (bpDispatcher)
		QMetaObject::invokeMethod(bpDispatcher, "reload", Qt::QueuedConnection, Q_ARG(unsigned long, fileFrameDuration), Q_ARG(bool, isRecoverState));
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::reload(bool isRecoverState)
{
	if(bpDispatcher)
		QMetaObject::invokeMethod(bpDispatcher, "reload", Qt::QueuedConnection, Q_ARG(bool, isRecoverState));
}

//------------------------------------------------------------------------------------------------------------------------------
BramPlayer::EPlayerState BramPlayerWidget::state(void)
{
	if (!bpDispatcher)
        return BramPlayer::EPlayerState::ePSStopped;
	
	return bpDispatcher->state();
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::pause(void)
{
	if (bpDispatcher)
		bpDispatcher->pause();
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::play(void)
{
	if (bpDispatcher)
		bpDispatcher->play();
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::stop(void)
{
	if (bpDispatcher)
		bpDispatcher->stop();
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::seek(unsigned int pos)
{
	if (bpDispatcher)
		bpDispatcher->seek(pos);
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::seek(int pos)
{
	if (bpDispatcher)
        bpDispatcher->seek(pos);
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::setAudioScrub(bool bS)
{
    if (bpDispatcher)
        bpDispatcher->setAudioScrub(bS);
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::setDeinterlace(bool bS)
{
    if (bpDispatcher)
        bpDispatcher->setDeinterlace(bS);
}

//------------------------------------------------------------------------------------------------------------------------------
bool BramPlayerWidget::isVideoPresent(void)
{
	if (bpDispatcher)
		return bpDispatcher->isVideoPresent();

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
void BramPlayerWidget::setPeekmeterCount(int count)
{
	if (bpDispatcher)
		bpDispatcher->setPeekmeterCount(count);
}
