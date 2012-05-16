#ifndef __TOOLS_LOGGER_LOGGER_HPP__
#define __TOOLS_LOGGER_LOGGER_HPP__

#include "tools/logger/Writer.hpp"

namespace Tools { namespace Logger {

    template<class TLog>
    struct NullWriter
    {
    private:
        TLog& _parent;

    public:
        NullWriter(TLog& log, std::string const&) : _parent(log) {}

        void WriteFile(std::string const&) {}
        void Write(std::string const&) {}
        void WriteLine(std::string const&) {}

        NullWriter& operator <<(std::basic_ostream<char>& (*manip)(std::basic_ostream<char>&)) { this->_parent << manip; return *this; }
        NullWriter& operator <<(std::basic_ios<char>& (*manip)(std::basic_ios<char>&)) { this->_parent << manip; return *this; }
        NullWriter& operator <<(std::ios_base& (*manip)(std::ios_base&)) { this->_parent << manip; return *this; }
        template<class T>
        NullWriter& operator <<(T d)
        {
            this->_parent << d;
            return *this;
        }
        template<class T>
        int RegisterCallback(T callback) { return this->_parent.RegisterCallback(callback); }
        void UnregisterCallback(int callbackId) { this->_parent.UnregisterCallback(callbackId); }
    };

    template<>
    struct NullWriter<void>
    {
    public:
        NullWriter(std::string const&) {}
        NullWriter(std::ostream&, std::string const&) {}

        void WriteFile(std::string const&) {}
        void Write(std::string const&) {}
        void WriteLine(std::string const&) {}
        NullWriter& operator <<(std::basic_ostream<char>& (*)(std::basic_ostream<char>&)) { return *this; }
        NullWriter& operator <<(std::basic_ios<char>& (*)(std::basic_ios<char>&)) { return *this; }
        NullWriter& operator <<(std::ios_base& (*)(std::ios_base&)) { return *this; }
        template<class T> NullWriter& operator <<(T) { return *this; }
        template<class T>
        void RegisterCallback(T) {}
        void UnregisterCallback(int) {}
    };

    typedef Writer<void> Logger;
    typedef NullWriter<void> NullLogger;

#ifdef DEBUG
    typedef Logger DebugLogger;
#else
    typedef NullLogger DebugLogger;
#endif

    extern DebugLogger debug;
    extern Writer<DebugLogger> log;
    extern Writer<Writer<DebugLogger>> error;

}}

namespace Tools {
    // Permet d'ecrire Tools::log << "toto" << std::endl;
    using Tools::Logger::log;
    using Tools::Logger::error;
    using Tools::Logger::debug;
}

#endif
