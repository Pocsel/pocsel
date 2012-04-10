#include "tools/plugin/Plugin.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Tools { namespace Plugin {

    bool Create(boost::filesystem::path const& pluginRoot, boost::filesystem::path const& destFile, std::ostream& log)
    {
        // check de base des chemins
        if (pluginRoot.has_filename())
        {
            log << "\"" << pluginRoot << "\" is not a valid plugin root directory." << std::endl;
            return false;
        }
        if (!destFile.has_filename())
        {
            log << "\"" << destFile << "\" is not a valid plugin destination file." << std::endl;
            return false;
        }

        // fichier de conf du plugin
        std::string identifier;
        std::string fullname;
        try
        {
            Tools::Lua::Interpreter i;
            i.DoFile((pluginRoot / "plugin.lua").string());
            Tools::Lua::Ref ref(i.GetState());
            ref = i.Globals()["identifier"];
            if (ref.Exists())
                identifier = ref.CheckString("identifier must be a string");
            else
                throw std::runtime_error("configuration string identifier not found");
            ref = i.Globals()["fullname"];
            if (ref.Exists())
                fullname = ref.CheckString("fullname must be a string");
            else
                fullname = identifier;
        }
        catch (std::exception& e)
        {
            log << "Failed to read plugin configuration file: " << e.what() << std::endl;
            return false;
        }
        log << "Loading plugin \"" << identifier << "\" (fullname: \"" << fullname << "\")." << std::endl;

        return true;
    }

    bool Install(/* TODO */ std::ostream& log)
    {
        return true;
    }

}}
