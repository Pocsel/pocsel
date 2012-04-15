#include <boost/uuid/uuid_io.hpp>

#include "server/rcon/SessionManager.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "server/Settings.hpp"

namespace Server { namespace Rcon {

    SessionManager::SessionManager(Settings const& settings) :
        _settings(settings),
        _tokenGenerator()
    {
    }

    std::string SessionManager::NewSession(std::string const& login, std::string const& password, std::string const& userAgent, std::list<std::string>& rights)
    {
        this->_ExpireTokens();
        // TODO retourner le meme token pour qqn de deja connecté
        auto it = this->_settings.rconUsers.find(login);
        if (it != this->_settings.rconUsers.end() && it->second.password == password && !it->second.password.empty())
        {
            Session s;
            s.login = login;
            s.userAgent = userAgent;
            std::string token = boost::uuids::to_string(this->_tokenGenerator());
            this->_sessions[token] = s;
            rights = it->second.rights;
            return token;
        }
        return std::string(); // flag pour foirage
    }

    bool SessionManager::HasRights(std::string const& token, std::string const& rights)
    {
        this->_ExpireTokens();
        auto it = this->_sessions.find(token);
        if (it != this->_sessions.end())
        {
            auto itUser = this->_settings.rconUsers.find(it->second.login);
            if (itUser != this->_settings.rconUsers.end())
            {
                auto itRights = itUser->second.rights.begin();
                auto itRightsEnd = itUser->second.rights.end();
                for (; itRights != itRightsEnd; ++itRights)
                    if (*itRights == rights)
                        return true;
            }
        }
        return false;
    }

    void SessionManager::_ExpireTokens()
    {
        // TODO expiration des sessions
    }

    std::string SessionManager::RconGetSessions() const
    {
        std::string json = "[\n";
        auto it = this->_sessions.begin();
        auto itEnd = this->_sessions.end();
        for (; it != itEnd; ++it)
        {
            if (it != this->_sessions.begin())
                json += ",\n";
            json +=
                "\t{\n"
                "\t\t\"login\": \"" + ToJsonStr(it->second.login) + "\",\n" +
                "\t\t\"user_agent\": \"" + ToJsonStr(it->second.userAgent) + "\",\n" +
                "\t}";
        }
        json += "\n]\n";
        return json;
    }

}}
