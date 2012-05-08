#include "common/FieldValidator.hpp"

namespace Common {

    bool FieldValidator::IsAlNum(std::string const& str)
    {
        return FieldValidator::_IsAlNum(str, "");
    }

    bool FieldValidator::IsAlNumDash(std::string const& str)
    {
        return FieldValidator::_IsAlNum(str, "-");
    }

    bool FieldValidator::IsAlNumUnderscore(std::string const& str)
    {
        return FieldValidator::_IsAlNum(str, "_");
    }

    bool FieldValidator::IsAlNumDashUnderscore(std::string const& str)
    {
        return FieldValidator::_IsAlNum(str, "-_");
    }

    bool FieldValidator::IsPluginIdentifier(std::string const& str)
    {
        return FieldValidator::IsAlNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldValidator::IsWorldIdentifier(std::string const& str)
    {
        return FieldValidator::IsAlNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldValidator::IsPluginFullname(std::string const& str)
    {
        return str.size() <= 100;
    }

    bool FieldValidator::IsWorldFullname(std::string const& str)
    {
        return str.size() <= 100;
    }

    bool FieldValidator::IsEntityType(std::string const& str)
    {
        return FieldValidator::IsAlNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldValidator::IsMapName(std::string const& str)
    {
        return FieldValidator::IsAlNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldValidator::IsPluginBuildHash(std::string const& str)
    {
        return FieldValidator::IsAlNumDash(str) && str.size() >= 10 && str.size() <= 100; // regles arbitraires
    }

    bool FieldValidator::IsWorldBuildHash(std::string const& str)
    {
        return FieldValidator::IsAlNumDash(str) && str.size() >= 10 && str.size() <= 100; // regles arbitraires
    }

    bool FieldValidator::_IsAlNum(std::string const& str, std::string const& allowed)
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

}
