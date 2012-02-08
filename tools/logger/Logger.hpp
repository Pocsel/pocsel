#ifndef __TOOLS_LOGGER_LOGGER_HPP__
#define __TOOLS_LOGGER_LOGGER_HPP__

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include "tools/logger/Token.hpp"
#include "tools/logger/Writer.hpp"

namespace Tools { namespace Logger {

    struct NullWriter
    {
    protected:
        NullWriter(std::string const&) {}
        NullWriter(std::ostream&, std::string const&) {}
    public:
        void Write(std::string const&) {}
        void WriteLine(std::string const&) {}

        template<class T>
        NullWriter& operator <<(T)
        {
            return *this;
        }
    };

    template<class TWriter>
    class _Logger : public TWriter
    {
    public:
        explicit _Logger(std::string const& file)
            : TWriter(file)
        {
        }
        _Logger(std::ostream& outStream, std::string const& file = "")
            : TWriter(outStream, file)
        {
        }
    };

    typedef _Logger<Writer> Logger;
    typedef _Logger<NullWriter> NullLogger;
}}

#endif
