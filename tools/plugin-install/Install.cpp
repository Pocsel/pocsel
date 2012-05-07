#include "tools/plugin-install/Install.hpp"
#include "common/Constants.hpp"

namespace Tools { namespace PluginInstall {

    bool Install(boost::filesystem::path const& pluginFile, boost::filesystem::path const& worldFile)
    {
        // check de base des chemins
        if (!boost::filesystem::is_regular_file(pluginFile))
        {
            Tools::log << "Error: " << pluginFile << " is not a regular file." << std::endl;
            return false;
        }
        if (boost::filesystem::is_regular_file(worldFile))
        {
            Tools::log << "Error: " << worldFile << " is not a regular file." << std::endl;
            return false;
        }

        // ouverture plugin
        std::string pluginIdentifier;
        std::string pluginFullname;
        std::string pluginBuildHash;
        Tools::Database::IConnection* pconn = 0;
        try
        {
            pconn = new Tools::Database::Sqlite::Connection(pluginFile.string());
            auto query = pconn->CreateQuery("SELECT build_hash, fullname, identifier, format_version FROM plugin");
            if (auto row = query->Fetch())
            {
                if (row->GetUint32(3) != Common::PluginFormatVersion)
                    throw std::runtime_error("bad format version (got " + Tools::ToString(row->GetUint32(3)) + ", expected " + Tools::ToString(Common::PluginFormatVersion) + ")");
            }
            else
                throw std::runtime_error("no metatada");
        }
        catch (std::exception& e)
        {
            Tools::log << "Error when opening plugin: " << e.what() << std::endl;
            Tools::Delete(pconn);
            return false;
        }

        // ouverture world
        std::string worldIdentifier;
        std::string worldFullname;
        Tools::Database::IConnection* wconn = 0;
        try
        {
            wconn = new Tools::Database::Sqlite::Connection(worldFile.string());
        }
        catch (std::exception& e)
        {
            Tools::log << "Error when opening world: " << e.what() << std::endl;
            Tools::Delete(wconn);
            return false;
        }

        // installation
        Tools::log << "Installing plugin " << pluginFile << " (\"" << pluginIdentifier << "\", \"" << pluginFullname << "\") in world " << worldFile << " (\"" << worldIdentifier << "\", \"" << worldFullname << "\")." << std::endl;
        try
        {
            wconn->BeginTransaction();


            Tools::log << "Finishing writing to database..." << std::endl;
            wconn->EndTransaction();
            Tools::Delete(pconn);
            Tools::Delete(wconn);
        }
        catch (std::exception& e)
        {
            Tools::log << "Error when installing: " << e.what() << std::endl;
            Tools::Delete(pconn);
            Tools::Delete(wconn);
            return false;
        }

        // success!
        Tools::log << "Plugin " << pluginFile << " (\"" << pluginIdentifier << "\", \"" << pluginFullname << "\") successfully installed in world " << worldFile << " (\"" << worldIdentifier << "\", \"" << worldFullname << "\")." << std::endl;
        return true;
    }

    bool Uninstall(std::string const& pluginIdentifier, boost::filesystem::path const& worldFile)
    {
    }

}}
