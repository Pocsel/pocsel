#ifndef __CLIENT_CHUNK_HPP__
#define __CLIENT_CHUNK_HPP__

#include "ProgramInfo.hpp"
#include "tools/logger/Logger.hpp"

namespace Tools { namespace Logger {

    DebugLogger debug(std::cout, PROGRAM_NAME "_debug.log");
    Writer<DebugLogger> log(debug, std::cout, PROGRAM_NAME ".log");
    Writer<Writer<DebugLogger>> error(log, std::cerr, PROGRAM_NAME "_error.log");

    namespace {
        struct Start
        {
            Start()
            {
                log << " == " PROJECT_NAME "-" PROGRAM_NAME " version: " GIT_VERSION " ==\n";
            }
        } _;
    }
}}
#endif
