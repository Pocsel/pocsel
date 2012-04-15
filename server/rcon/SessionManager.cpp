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
        // TODO check login/pass + retourner le meme token pour qqn de deja connecté
        // retourner une chaine vide quand login/pass invalides
        std::string token = boost::uuids::to_string(this->_tokenGenerator());
        this->_sessions[token] = login;
        return token;
    }

    bool SessionManager::HasRights(std::string const& token, std::string const& rights)
    {
        this->_ExpireTokens();
        // TODO checker les droits en plus du token
        auto it = this->_sessions.find(token);
        if (it != this->_sessions.end())
            return true;
        return false;
    }

    void SessionManager::_ExpireTokens()
    {
        // TODO expiration des sessions
    }

}}
