#include "common/FieldValidator.hpp"

namespace Common {

    static bool FieldValidator::IsAlNum(std::string const& str)
    {
        return this->_IsAlNum(str, "");
    }

    static bool FieldValidator::IsAlNumDash(std::string const& str)
    {
        return this->_IsAlNum(str, "-");
    }

    static bool FieldValidator::IsAlNumUnderscore(std::string const& str)
    {
        return this->_IsAlNum(str, "_");
    }

    static bool FieldValidator::IsAlNumDashUnderscore(std::string const& str)
    {
        return this->_IsAlNum(str, "-_");
    }

    static bool FieldValidator::IsPluginIdentifier(std::string const& str)
    {
        return this->IsAlNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    static bool FieldValidator::IsPluginFullname(std::string const& str)
    {
        return str.size() <= 100;
    }

    static bool FieldValidator::IsEntityType(std::string const& str)
    {
        return this->IsAlNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    static bool FieldValidator::IsMapName(std::string const& str)
    {
        return this->IsAlNumDashUnderscore(str) && str.size() >= 1 && str.size() <= 20;
    }

    bool FieldValidator::_IsAlNum(std::string const& str, std::string const& allowed)
    {
        auto it = str.begin();
        auto itEnd = str.end();
        for (; it != itEnd; ++it)
            if (!isalnum(*it))
            {
                auto itAllowed = allowed.begin();
                auto itAllowedEnd = allowed.end();
                for (; itAllowed != itAllowedEnd; ++itAllowed)
                    if (*it == *itAllowed)
                        continue;
                return false;
            }
        return true;
    }

}
