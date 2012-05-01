#include <boost/uuid/uuid_io.hpp>

#include "server/rcon/SessionManager.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "server/Settings.hpp"

namespace Server { namespace Rcon {

    SessionManager::SessionManager(Settings const& settings) :
        _settings(settings),
        _tokenGenerator()
    {
        if (this->_settings.debug)
            this->_logLines = LogLinesDebug;
        else
            this->_logLines = LogLines;
        Tools::debug.RegisterCallback(std::bind(&SessionManager::_Log, this, std::placeholders::_1, Normal));
        Tools::log.RegisterCallback(std::bind(&SessionManager::_Log, this, std::placeholders::_1, Normal));
        Tools::error.RegisterCallback(std::bind(&SessionManager::_Log, this, std::placeholders::_1, Error));
    }

    std::string SessionManager::NewSession(std::string const& login, std::string const& password, std::string const& userAgent, std::list<std::string>& rights)
    {
        boost::unique_lock<boost::shared_mutex> lock(this->_lock);
        this->_ExpireTokens();
        auto it = this->_settings.rconUsers.find(login);
        if (it != this->_settings.rconUsers.end() && it->second.password == password && !it->second.password.empty())
        {
            rights = it->second.rights; // retourne la liste des droits du user par reference

            // trouve si cette session va bufferiser les logs
            bool logs = false;
            auto itRights = it->second.rights.begin();
            auto itRightsEnd = it->second.rights.end();
            for (; itRights != itRightsEnd; ++itRights)
                if (*itRights == "logs")
                {
                    logs = true;
                    break;
                }

            // ne crée pas de nouvelle session si le type était deja loggé (mais update son user-agent)
            auto itSession = this->_sessions.begin();
            auto itSessionEnd = this->_sessions.end();
            for (; itSession != itSessionEnd; ++itSession)
                if (itSession->second.login == login)
                {
                    itSession->second.logs = logs; // pas forcement necessaire ? osef
                    itSession->second.userAgent = userAgent;
                    return itSession->first;
                }

            // le mec n'était pas loggé, nouvelle session
            Session s;
            s.login = login;
            s.userAgent = userAgent;
            s.logs = logs;
            std::string token = boost::uuids::to_string(this->_tokenGenerator());
            this->_sessions[token] = s;
            return token;
        }
        return std::string(); // flag "" pour foirage
    }

    bool SessionManager::HasRights(std::string const& token, std::string const& rights)
    {
        boost::unique_lock<boost::shared_mutex> lock(this->_lock);
        this->_ExpireTokens(token);
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

    void SessionManager::_Log(std::string const& message, LogLevel level)
    {
        boost::upgrade_lock<boost::shared_mutex> lock(this->_lock);
        if (!this->_sessions.empty())
        {
            boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
            this->_ExpireTokens();
            auto it = this->_sessions.begin();
            auto itEnd = this->_sessions.end();
            for (; it != itEnd; ++it)
            {
                if (!it->second.logs)
                    continue;
                it->second.normalLog.push_back(message);
                if (it->second.normalLog.size() > this->_logLines)
                    it->second.normalLog.pop_front();
                if (level == Error)
                {
                    it->second.errorLog.push_back(message);
                    if (it->second.errorLog.size() > this->_logLines)
                        it->second.errorLog.pop_front();
                }
            }
        }
    }

    void SessionManager::_ExpireTokens(std::string const& recentToken /* = "" */)
    {
        // TODO expiration des sessions
    }

    std::string SessionManager::RconGetLogs(std::string const& token)
    {
        boost::unique_lock<boost::shared_mutex> lock(this->_lock);
        std::string json = "{\n";
        auto it = this->_sessions.find(token);
        if (it != this->_sessions.end())
        {
            json += "\t\"normal\": \"";
            {
                auto itLog = it->second.normalLog.begin();
                auto itLogEnd = it->second.normalLog.end();
                for (; itLog != itLogEnd; ++itLog)
                    json += ToJsonStr(*itLog);
                it->second.normalLog.clear();
            }
            json += "\",\n";
            json += "\t\"error\": \"";
            {
                auto itLog = it->second.errorLog.begin();
                auto itLogEnd = it->second.errorLog.end();
                for (; itLog != itLogEnd; ++itLog)
                    json += ToJsonStr(*itLog);
                it->second.errorLog.clear();
            }
            json += "\"\n";
        }
        json += "}\n";
        return json;
    }

    std::string SessionManager::RconGetSessions() const
    {
        boost::shared_lock<boost::shared_mutex> lock(this->_lock);
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
                "\t\t\"user_agent\": \"" + ToJsonStr(it->second.userAgent) + "\"\n" +
                "\t}";
        }
        json += "\n]\n";
        return json;
    }

}}
