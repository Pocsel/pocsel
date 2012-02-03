#ifdef _WIN32 //windows
# include <windows.h>
#else //unix
# include <sys/time.h>
# include <unistd.h>
#endif

#include "tools/Timer.hpp"

namespace Tools {

    Timer::Timer()
    {
        this->Reset();
    }

    void Timer::Reset()
    {
#ifdef _WIN32 //windows
        ::QueryPerformanceFrequency(&this->_frequence);
        LARGE_INTEGER curCount;
        ::QueryPerformanceCounter(&curCount);
        this->_base_time = curCount.QuadPart;
#else //unix
        struct timeval tv;
        ::gettimeofday(&tv, 0);
        this->_base_time = tv.tv_sec;
        this->_base_time *= 1000000;
        this->_base_time += tv.tv_usec;
#endif
    }

    Uint32 Timer::GetElapsedTime() const
    {
#ifdef _WIN32 //windows
        LARGE_INTEGER curCount;
        ::QueryPerformanceCounter(&curCount);
        return static_cast<Uint32>((curCount.QuadPart - this->_base_time) * 1000 / this->_frequence.QuadPart);
#else //unix
        struct timeval tv;
        ::gettimeofday(&tv, 0);
        Uint64 nu_time;
        nu_time = tv.tv_sec;
        nu_time *= 1000;
        nu_time += tv.tv_usec / 1000;
        return nu_time - this->_base_time / 1000;
#endif
    }

    Uint64 Timer::GetPreciseElapsedTime() const
    {
#ifdef _WIN32 //windows
        LARGE_INTEGER curCount;
        ::QueryPerformanceCounter(&curCount);
        return (curCount.QuadPart - this->_base_time) * 1000000 / this->_frequence.QuadPart;
#else //unix
        struct timeval tv;
        ::gettimeofday(&tv, 0);
        Uint64 nu_time;
        nu_time = tv.tv_sec;
        nu_time *= 1000000;
        nu_time += tv.tv_usec;
        return nu_time - this->_base_time;
#endif
    }

    void Timer::Sleep(Uint32 ms)
    {
#ifdef _WIN32
        ::Sleep(ms);
#else // Unix
        ::usleep(ms * 1000L);
#endif
    }

}
