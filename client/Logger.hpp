#ifndef __CLIENT_LOGGER_HPP__
#define __CLIENT_LOGGER_HPP__

#include "tools/logger/Logger.hpp"

namespace Client {

#ifdef DEBUG
    typedef Tools::Logger::Logger _CoutLogger;
    typedef Tools::Logger::Logger _DebugLogger;
#else
    typedef Tools::Logger::Logger _CoutLogger;
    typedef Tools::Logger::NullLogger _DebugLogger;
#endif

    extern _CoutLogger cout;
    extern _DebugLogger debug;

}

#endif
