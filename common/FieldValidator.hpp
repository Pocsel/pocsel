#ifndef __COMMON_FIELDVALIDATOR_HPP__
#define __COMMON_FIELDVALIDATOR_HPP__

namespace Common {

    class FieldValidator
    {
    public:
        static bool IsAlphaNum(std::string const& str);
        static bool IsAlphaNumDash(std::string const& str);
        static bool IsAlphaNumUnderscore(std::string const& str);
        static bool IsAlphaNumDashUnderscore(std::string const& str);
        static bool IsPluginIdentifier(std::string const& str);
        static bool IsWorldIdentifier(std::string const& str);
        static bool IsPluginFullname(std::string const& str);
        static bool IsWorldFullname(std::string const& str);
        static bool IsRegistrableType(std::string const& str);
        static bool IsMapName(std::string const& str);
        static bool IsPluginBuildHash(std::string const& str);
        static bool IsWorldBuildHash(std::string const& str);
    private:
        static bool _IsAlphaNum(std::string const& str, std::string const& allowed);
    };

}

#endif
