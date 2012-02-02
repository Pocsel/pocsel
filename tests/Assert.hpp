#ifndef __ASSERT_HPP__
#define __ASSERT_HPP__

#include <iostream>

static size_t _count = 0, _failures = 0;

static void _PrintResult()
{
    std::cout << _count - _failures << ' ' << _count << std::endl;
}

static int __dummy = (int) atexit(&_PrintResult);

static void _Assert(char const* expr, char const* file, size_t line, char const* func)
{
    (void) __dummy;
    std::cerr //<< "Test " << _count << " failed: "
              << file << ':' << line << " in "
              << func << "(): " << expr << " Failed" << std::endl;
    ++_failures;
}

#ifdef Assert
# undef Assert
#endif

#ifdef _WIN32
#define Assert(x) (void)((++_count && (x)==0) ?_Assert(#x,__FILE__,__LINE__,__FUNCTION__):(void)0)
#else
#define Assert(x) (void)((++_count && (x)==0) ?_Assert(#x,__FILE__,__LINE__,__func__):(void)0)
#endif
#endif
