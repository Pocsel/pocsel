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
        Settings const& _settings;
        boost::uuids::random_generator _tokenGenerator;

    public:
        SessionManager(Settings const& settings);
        std::string NewSession(std::string const& login, std::string const& password);
        bool HasRights(std::string const& token, std::string const& rights);
    private:
        void _ExpireTokens();
    };

}}

#endif
