#ifndef __SERVER_RCON_SESSIONMANAGER_HPP__
#define __SERVER_RCON_SESSIONMANAGER_HPP__

#include <boost/uuid/random_generator.hpp>

#include "tools/Timer.hpp"

namespace Server {
    class Settings;
}

namespace Server { namespace Rcon {

    class SessionManager :
        private boost::noncopyable
    {
    private:
        enum LogLevel
        {
            Normal,
            Error,
        };
        enum
        {
            LogLines = 100,
            LogLinesDebug = 1000,
            SessionTimeout = 20, // seconds
        };
        struct Session
        {
            Tools::Timer timer;
            std::string login;
            std::string userAgent;
            bool logs; // true si le user à le droit "logs" (= il faut bufferiser les logs)
            std::list<std::string> errorLog;
            std::list<std::string> normalLog;
        };

    private:
        Settings const& _settings;
        boost::uuids::random_generator _tokenGenerator;
        std::map<std::string /* token */, Session> _sessions; // protégé par _lock
        unsigned int _logLines;
        mutable boost::shared_mutex _lock;
        int _debugCallback;
        int _logCallback;
        int _errorCallback;

    public:
        SessionManager(Settings const& settings);
        ~SessionManager();
        std::string NewSession(std::string const& login, std::string const& password, std::string const& userAgent, std::list<std::string>& rights);
        bool HasRights(std::string const& token, std::string const& rights);
        std::string RconGetLogs(std::string const& token);
        std::string RconGetSessions() const;
    private:
        void _Log(std::string const& message, LogLevel level);
        void _ExpireTokens(std::string const& recentToken = "");
    };

}}

#endif
