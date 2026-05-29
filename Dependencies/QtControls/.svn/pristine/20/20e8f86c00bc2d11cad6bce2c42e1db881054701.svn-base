#ifndef _TCOPERATIONS_H_
#define _TCOPERATIONS_H_

#include <QString>
#include <chrono>

typedef unsigned long TC;

// Структура которая хранит значение таймкода и частоту кадров
// Эта информация необходима для корректного отображения таймкода
struct Timecode {
    TC tc;
    int famerate;

    Timecode()
        : tc(0)
        , famerate(25)
    {
    }
    Timecode(TC t, int fr)
        : tc(t)
        , famerate(fr)
    {
    }
    // Изменить на значение в мсек
    Timecode deltaMsec(int msec)
    {
        Timecode dtc(*this);
        dtc.tc += msec / famerate;
        return dtc;
    }
};

namespace TCOperations {
//-----------------------------------------------------------------------------------------------
// Проверка значения ТC на валидность
TC checkTC(TC tc, unsigned int frameRate = 25);
//-----------------------------------------------------------------------------------------------
// Преобразование из строки в TC
TC strToTC(QString str, unsigned int frameRate = 25);
//-----------------------------------------------------------------------------------------------
// Преобразование интервела FILETIME к миллисекундам
inline quint64 fileTimeToMsec(quint64 ft)
{
    return ft / 10000;
}
// Преобразование интервела FILETIME к миллисекундам
inline quint64 msecToFileTime(quint64 msec)
{
    return msec * 10000;
}
//-----------------------------------------------------------------------------------------------
// Преобразование из TC в строку
QString getTCAsString(TC value, int frameRate = 25);
QString getTCAsString(const Timecode& tc);
//-----------------------------------------------------------------------------------------------
// Преобразование TC в filetime(100нс интервалы)
inline quint64 tcToFileTime(TC tc, int framerate = 25)
{
    return ((quint64)tc / framerate + tc % framerate / framerate) * 1000 * 10000;
}
//-----------------------------------------------------------------------------------------------
// Преобразование TC в мс
inline quint64 tcToMS(TC tc, int framerate = 25)
{
    return ((quint64)tc / framerate) * 1000 + ((double)(tc % framerate) / (double)framerate) * 1000.0;
}
//-----------------------------------------------------------------------------------------------
// Преобразование мс в TC
inline TC msToTC(quint64 ms, const int framerate = 25)
{
    return /*находим кол-во кадров*/ ms % 1000 / (1000 / framerate) + static_cast<TC>(ms / 1000) * framerate;
}
//-----------------------------------------------------------------------------------------------
// Преобразование filetime(100нс интервалы) в TC
inline TC fileTimeToTC(quint64 ft, int framerate = 25)
{
    return msToTC(ft / 10000, framerate);
}
//-----------------------------------------------------------------------------------------------
using namespace std::chrono;
inline time_point<system_clock> fileTimeToChronoSystemClock(uint64_t ft)
{
    // start epoch filetime(Jan 1 1601) to system_time(Jan 1 1970) in sec
    // const uint64_t deltaSecEpoches = 11644473600;
    // Jan 1 1601 from start filetime epoch
    return time_point<system_clock> { time_point<system_clock>::duration((ft - 116444736000000000) * 100) };
}
//-----------------------------------------------------------------------------------------------
inline uint64_t chronoSystemClockToFileTime(time_point<system_clock> sc)
{
    // start epoch filetime(Jan 1 1601) to system_time(Jan 1 1970) in sec
    // const uint64_t deltaSecEpoches = 11644473600;
    // Jan 1 1601 from start filetime epoch
    return (std::chrono::duration_cast<std::chrono::seconds>(sc.time_since_epoch()).count() + 11644473600) * 10000000;
}
//-----------------------------------------------------------------------------------------------
// Преобразование TC в std::chrono::time_point<system_time>
inline time_point<system_clock>::duration tcToChronoSystemClock(TC tc, int framerate = 25)
{
    return time_point<system_clock>::duration((tc / framerate + tc % framerate / framerate) * 1000);
}
//-----------------------------------------------------------------------------------------------
// Преобразование td::chrono::time_point<system_time> в TC
inline TC chronoSystemClockDurationToTC(time_point<system_clock>::duration dur, const int framerate = 25)
{
    return msToTC(duration_cast<milliseconds>(dur).count(), framerate);
}
}

#endif // !_TCOPERATIONS_H_
