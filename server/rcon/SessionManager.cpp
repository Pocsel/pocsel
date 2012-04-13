#include <boost/uuid/uuid_io.hpp>

#include "server/rcon/SessionManager.hpp"
#include "server/Settings.hpp"

namespace Server { namespace Rcon {

    SessionManager::SessionManager(Settings const& settings) :
        _settings(settings)
    {
    }

    std::string SessionManager::NewSession(std::string const& login, std::string const& password)
    {
        this->_ExpireTokens();
        return boost::uuids::to_string(this->_tokenGenerator());
    }

    bool SessionManager::HasRights(std::string const& token, std::string const& rights)
    {
        this->_ExpireTokens();
        return false;
    }

    void SessionManager::_ExpireTokens()
    {
    }

}}
