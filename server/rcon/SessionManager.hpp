#ifndef __SERVER_RCON_SESSIONMANAGER_HPP__
#define __SERVER_RCON_SESSIONMANAGER_HPP__

#include <boost/uuid/random_generator.hpp>

namespace Server {
    class Settings;
}

namespace Server { namespace Rcon {

    class SessionManager :
        private boost::noncopyable
    {
    private:
        struct Session
        {
            std::string login;
            std::string userAgent;
        };

    private:
        Settings const& _settings;
        boost::uuids::random_generator _tokenGenerator;
        std::map<std::string /* token */, Session> _sessions;

    public:
        SessionManager(Settings const& settings);
        std::string NewSession(std::string const& login, std::string const& password, std::string const& userAgent, std::list<std::string>& rights);
        bool HasRights(std::string const& token, std::string const& rights);
        std::string RconGetSessions() const;
    private:
        void _ExpireTokens(std::string const& recentToken = "");
    };

}}

#endif
