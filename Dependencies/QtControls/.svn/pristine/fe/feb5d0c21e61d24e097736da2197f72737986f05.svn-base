#include "TCOperations.h"

//-----------------------------------------------------------------------------------------------
TC TCOperations::checkTC(TC tc, unsigned int frameRate)
{
    Q_ASSERT(frameRate > 0);

    if (!frameRate)
        return 0;
    else if (tc > 100 * 60 * 60 * frameRate - 1)
        return 100 * 60 * 60 * frameRate - 1;
    else
        return tc;
}
//-----------------------------------------------------------------------------------------------
// Преобразование из строки в TC
TC TCOperations::strToTC(QString str, unsigned int frameRate)
{
    Q_ASSERT(frameRate > 0);

    char b[4] = { 0 };
    b[0] = str.section(":", 3).toInt();
    b[1] = str.section(":", 2, 2).toInt();
    b[2] = str.section(":", 1, 1).toInt();
    b[3] = qAbs(str.section(":", 0, 0).toInt());

    return b[3] * 60 * 60 * frameRate + b[2] * 60 * frameRate + b[1] * frameRate + b[0];
}
//-----------------------------------------------------------------------------------------------
// Преобразование из TC в строку
QString TCOperations::getTCAsString(TC value, int frameRate)
{
    Q_ASSERT(frameRate > 0);

    if (!frameRate)
        return QString("zero framerate");

    value = checkTC(value);

    char b[4] = { 0 };
    b[0] = (char)(value % static_cast<int>(frameRate));
    b[1] = (char)(static_cast<int>(value / frameRate) % 60);
    b[2] = (char)(static_cast<int>(value / frameRate / 60) % 60);
    b[3] = (char)(static_cast<int>(value / frameRate / 60 / 60) % 100);

    return QString("%1:%2:%3:%4").arg(b[3], 2, 10, (QLatin1Char)'0').arg(b[2], 2, 10, QLatin1Char('0')).
        arg(b[1], 2, 10, (QLatin1Char)'0').arg(b[0], 2, 10, (QLatin1Char)'0');
}

QString TCOperations::getTCAsString(const Timecode &tc)
{
	return getTCAsString(tc.tc, tc.famerate);
}
