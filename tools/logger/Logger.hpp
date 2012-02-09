#ifndef __TOOLS_LOGGER_LOGGER_HPP__
#define __TOOLS_LOGGER_LOGGER_HPP__

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include "tools/logger/Token.hpp"
#include "tools/logger/Writer.hpp"

namespace Tools { namespace Logger {

    template<class TLog>
    struct NullWriter
    {
    private:
        TLog& _parent;

    protected:
        NullWriter(TLog& log, std::string const&) : _parent(log) {}

    public:
        void WriteFile(std::string const&) {}
        void Write(std::string const&) {}
        void WriteLine(std::string const&) {}

        template<class T>
        NullWriter& operator <<(T d)
        {
            this->_parent << d;
            return *this;
        }
    };

    template<>
    struct NullWriter<void>
    {
    protected:
        NullWriter(std::string const&) {}
        NullWriter(std::ostream&, std::string const&) {}

    public:
        void WriteFile(std::string const&) {}
        void Write(std::string const&) {}
        void WriteLine(std::string const&) {}
        template<class T> NullWriter& operator <<(T) { return *this; }
    };

    template<class TWriter>
    class _Logger : public TWriter
    {
    public:
        typedef TWriter Writer;

        explicit _Logger(std::string const& file)
            : TWriter(file)
        {
        }

        _Logger(std::ostream& outStream, std::string const& file)
            : TWriter(outStream, file)
        {
        }

        template<class TLog>
        _Logger(TLog& parent, std::string const& file)
            : TWriter(parent, file)
        {
        }

        template<class TLog>
        _Logger(TLog& parent, std::ostream& outStream, std::string const& file)
            : TWriter(parent, outStream, file)
        {
        }
    };

    typedef _Logger<Writer<void>> Logger;
    typedef _Logger<NullWriter<void>> NullLogger;

#ifdef DEBUG
    typedef Logger DebugLogger;
#else
    typedef NullLogger DebugLogger;
#endif

    extern DebugLogger debug;
    extern _Logger<Writer<DebugLogger::Writer>> log;
    extern _Logger<Writer<_Logger<Writer<DebugLogger::Writer>>::Writer>> error;

}}

namespace Tools {
    // Permet d'ecrire Tools::log << "toto" << Tools::endl;
    using Tools::Logger::log;
    using Tools::Logger::error;
    using Tools::Logger::debug;
}

#endif
