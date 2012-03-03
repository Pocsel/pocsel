#ifndef __TOOLS_TIMER_HPP__
#define __TOOLS_TIMER_HPP__

#ifdef WIN32
# include "windows.h"
#endif

namespace Tools {

    class Timer
    {
    private:
        Uint64 _base_time;
#ifdef _WIN32
        LARGE_INTEGER _frequence;
#endif // _WIN32

    public:
        Timer();
        void Reset();
        /**
          * Takes time in microseconds
          */
        void Set(Uint64 time);
        /**
         * Returns elapsed time in milliseconds
         */
        Uint32 GetElapsedTime() const;
        /**
         * Returns elapsed time in microseconds
         */
        Uint64 GetPreciseElapsedTime() const;
        static void Sleep(Uint32 ms);
    };

}

#endif
