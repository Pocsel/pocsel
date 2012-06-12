#include "common/FieldUtils.hpp"

namespace Common {

    bool FieldUtils::IsAlphaNum(std::string const& str)
    {
        return FieldUtils::_IsAlphaNum(str, "");
    }

    bool FieldUtils::IsAlphaNumDash(std::string const& str)
    {
        return FieldUtils::_IsAlphaNum(str, "-");
    }

    bool FieldUtils::IsAlphaNumUnderscore(std::string const& str)
    {
        return FieldUtils::_IsAlphaNum(str, "_");
    }

    bool FieldUtils::IsAlphaNumDashUnderscore(std::string const& str)
    {
        return FieldUtils::_IsAlphaNum(str, "-_");
    }

    bool FieldUtils::IsPluginIdentifier(std::string const& str)
    {
        return FieldUtils::IsAlphaNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldUtils::IsWorldIdentifier(std::string const& str)
    {
        return FieldUtils::IsAlphaNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldUtils::IsPluginFullname(std::string const& str)
    {
        return str.size() <= 100;
    }

    bool FieldUtils::IsWorldFullname(std::string const& str)
    {
        return str.size() <= 100;
    }

    bool FieldUtils::IsRegistrableType(std::string const& str)
    {
        return FieldUtils::IsAlphaNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldUtils::IsMapName(std::string const& str)
    {
        return FieldUtils::IsAlphaNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldUtils::IsPluginBuildHash(std::string const& str)
    {
        return FieldUtils::IsAlphaNumDash(str) && str.size() >= 10 && str.size() <= 100; // regles arbitraires
    }

    bool FieldUtils::IsWorldBuildHash(std::string const& str)
    {
        return FieldUtils::IsAlphaNumDash(str) && str.size() >= 10 && str.size() <= 100; // regles arbitraires
    }

    bool FieldUtils::_IsAlphaNum(std::string const& str, std::string const& allowed)
    {
        auto it = str.begin();
        auto itEnd = str.end();
        for (; it != itEnd; ++it)
            if (!isalnum(*it))
            {
                bool found = false;
                auto itAllowed = allowed.begin();
                auto itAllowedEnd = allowed.end();
                for (; itAllowed != itAllowedEnd; ++itAllowed)
                    if (*it == *itAllowed)
                    {
                        found = true;
                        break;
                    }
                if (!found)
                    return false;
            }
        return true;
    }

    std::string FieldUtils::GetPluginNameFromResource(std::string const& name)
    {
        size_t colonPos = name.find_first_of(':');
        return colonPos == std::string::npos ? "" : name.substr(0, colonPos);
    }

    std::string FieldUtils::GetResourceNameFromResource(std::string const& name)
    {
        size_t colonPos = name.find_first_of(':');
        return colonPos == std::string::npos ? name : name.substr(colonPos + 1);
    }

    std::string FieldUtils::GetResourceName(std::string const& pluginName, std::string const& resourceName)
    {
        return pluginName + ":" + resourceName;
    }

}
