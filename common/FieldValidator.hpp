#ifndef __COMMON_FIELDVALIDATOR_HPP__
#define __COMMON_FIELDVALIDATOR_HPP__

namespace Common {

    class FieldValidator
    {
    public:
        static bool IsAlNum(std::string const& str);
        static bool IsAlNumDash(std::string const& str);
        static bool IsAlNumUnderscore(std::string const& str);
        static bool IsAlNumDashUnderscore(std::string const& str);
        static bool IsPluginIdentifier(std::string const& str);
        static bool IsPluginFullname(std::string const& str);
        static bool IsEntityType(std::string const& str);
        static bool IsMapName(std::string const& str);
    private:
        static bool _IsAlNum(std::string const& str, std::string const& allowed);
    };

}

#endif