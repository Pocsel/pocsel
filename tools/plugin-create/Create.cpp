#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/crc.hpp>

#include "tools/plugin-create/Create.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/database/sqlite/Connection.hpp"
#include "common/FieldValidator.hpp"

namespace boost { namespace filesystem3 {

    // pour MakeRelative() qui suit
    template<>
        path& path::append<typename path::iterator>(
                typename path::iterator begin,
                typename path::iterator end,
                const codecvt_type&)
        {
            for(; begin != end ; ++begin)
                *this /= *begin;
            return *this;
        }

}}

namespace Tools { namespace PluginCreate {

    namespace {

        // cf. http://stackoverflow.com/questions/10167382/boostfilesystem-get-relative-path
        // Return path when appended to a_From will resolve to same as a_To
        boost::filesystem::path MakeRelative( boost::filesystem::path a_From, boost::filesystem::path a_To )
        {
            a_From = boost::filesystem::absolute( a_From ); a_To = boost::filesystem::absolute( a_To );
            boost::filesystem::path ret;
            boost::filesystem::path::const_iterator itrFrom( a_From.begin() ), itrTo( a_To.begin() );
            // Find common base
            for (boost::filesystem::path::const_iterator toEnd( a_To.end() ), fromEnd( a_From.end() ) ; itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo);
            // Navigate backwards in directory to reach previously found base
            for (boost::filesystem::path::const_iterator fromEnd( a_From.end() ); itrFrom != fromEnd; ++itrFrom)
            {
                if( (*itrFrom) != "." )
                    ret /= "..";
            }
            // Now navigate down the directory branch
            ret.append(itrTo, a_To.end());
            return ret;
        }

        void RecursiveFileList(boost::filesystem::path const& directory, std::list<boost::filesystem::path>& list)
        {
            if (!boost::filesystem::is_directory(directory))
                throw std::runtime_error("Cannot browse directory " + directory.string() + ".");
            boost::filesystem::directory_iterator it(directory);
            boost::filesystem::directory_iterator itEnd;
            for (; it != itEnd; ++it)
                if (boost::filesystem::is_regular_file(*it))
                    list.push_back(*it);
                else if (boost::filesystem::is_directory(*it))
                    RecursiveFileList(*it, list);
        }

        void NonRecursiveFileList(boost::filesystem::path const& directory, std::list<boost::filesystem::path>& list)
        {
            if (!boost::filesystem::is_directory(directory))
                throw std::runtime_error("Cannot browse directory " + directory.string() + ".");
            boost::filesystem::directory_iterator it(directory);
            boost::filesystem::directory_iterator itEnd;
            for (; it != itEnd; ++it)
                if (boost::filesystem::is_regular_file(*it))
                    list.push_back(*it);
        }

        std::vector<char> ReadFile(boost::filesystem::path const& file)
        {
            boost::filesystem::ifstream tmp(file, std::ios::binary | std::ios::in);
            return std::vector<char>((std::istreambuf_iterator<char>(tmp)), std::istreambuf_iterator<char>());
        }

        std::string HashFile(std::vector<char> const& data)
        {
            boost::crc_32_type result;
            result.process_bytes(data.data(), data.size());
            return Tools::ToString(result.checksum());
        }

        void FillTablePlugin(Tools::Database::IConnection& conn, std::string const& identifier, std::string const& fullname)
        {
            boost::uuids::random_generator buildHashGenerator;
            conn.CreateQuery("INSERT INTO plugin (build_hash, fullname, identifier, format_version) VALUES (?, ?, ?, ?);")->
                Bind(boost::uuids::to_string(buildHashGenerator())).Bind(fullname).Bind(identifier).Bind(FormatVersion).ExecuteNonSelect();
        }

        void FillTableResource(boost::filesystem::path const& pluginRoot, Tools::Database::IConnection& conn)
        {
            boost::filesystem::path clientRoot(pluginRoot / "client");
            if (!boost::filesystem::is_directory(clientRoot))
            {
                Tools::log << clientRoot << " not found, no client files will be packaged." << std::endl;
                return;
            }

            std::map<std::string /* extension */, std::string /* type en base */> allowedTypes;
            allowedTypes[".lua"] = "lua";
            allowedTypes[".png"] = "image";
            allowedTypes[".fx"] = "effect";

            std::list<boost::filesystem::path> files;
            RecursiveFileList(clientRoot, files);
            auto it = files.begin();
            auto itEnd = files.end();
            for (; it != itEnd; ++it)
            {
                auto itType = allowedTypes.find(it->extension().string());
                if (itType == allowedTypes.end())
                {
                    Tools::log << "Client file " << *it << " ignored (unknown extension " << it->extension() << ")." << std::endl;
                    continue;
                }
                auto data = ReadFile(*it);
                std::string name = MakeRelative(clientRoot, *it).string();
                std::string type = itType->second;
                std::string hash = HashFile(data);
                conn.CreateQuery("INSERT INTO resource (name, type, data_hash, data) VALUES (?, ?, ?, ?);")->
                    Bind(name).Bind(type).Bind(hash).Bind(data.data(), data.size()).ExecuteNonSelect();
                Tools::log << "Added client file \"" << name << "\" (size: " << data.size() << " bytes, hash: " << hash << ", type: " << type << ")." << std::endl;
            }
        }

        void FillTableEntityFile(boost::filesystem::path const& pluginRoot, Tools::Database::IConnection& conn)
        {
            boost::filesystem::path entitiesRoot(pluginRoot / "entities");
            if (!boost::filesystem::is_directory(entitiesRoot))
            {
                Tools::log << entitiesRoot << " not found, no entity files will be packaged." << std::endl;
                return;
            }

            std::list<boost::filesystem::path> files;
            RecursiveFileList(entitiesRoot, files);
            auto it = files.begin();
            auto itEnd = files.end();
            for (; it != itEnd; ++it)
            {
                if (it->extension().string() != ".lua")
                {
                    Tools::log << "Entity file " << *it << " ignored (unknow extension " << it->extension() << ")." << std::endl;
                    continue;
                }
                auto data = ReadFile(*it);
                std::string name = MakeRelative(entitiesRoot, *it).string();
                conn.CreateQuery("INSERT INTO entity_file (name, lua) VALUES (?, ?);")->
                    Bind(name).Bind(std::string(data.begin(), data.end())).ExecuteNonSelect();
                Tools::log << "Added entity file \"" << name << "\" (size: " << data.size() << " bytes)." << std::endl;
            }
        }

        void FillTableCubeFile(boost::filesystem::path const& pluginRoot, Tools::Database::IConnection& conn)
        {
            boost::filesystem::path cubesRoot(pluginRoot / "cubes");
            if (!boost::filesystem::is_directory(cubesRoot))
            {
                Tools::log << cubesRoot << " not found, no cube files will be packaged." << std::endl;
                return;
            }

            std::list<boost::filesystem::path> files;
            RecursiveFileList(cubesRoot, files);
            auto it = files.begin();
            auto itEnd = files.end();
            for (; it != itEnd; ++it)
            {
                if (it->extension().string() != ".lua")
                {
                    Tools::log << "Cube file " << *it << " ignored (unknow extension " << it->extension() << ")." << std::endl;
                    continue;
                }
                auto data = ReadFile(*it);
                std::string name = MakeRelative(cubesRoot, *it).string();
                conn.CreateQuery("INSERT INTO cube_file (name, lua) VALUES (?, ?);")->
                    Bind(name).Bind(std::string(data.begin(), data.end())).ExecuteNonSelect();
                Tools::log << "Added cube file \"" << name << "\" (size: " << data.size() << " bytes)." << std::endl;
            }
        }

        void FillTableMap(boost::filesystem::path const& pluginRoot, Tools::Database::IConnection& conn)
        {
            boost::filesystem::path mapsRoot(pluginRoot / "maps");
            if (!boost::filesystem::is_directory(mapsRoot))
            {
                Tools::log << mapsRoot << " not found, no maps will be packaged." << std::endl;
                return;
            }

            std::list<boost::filesystem::path> files;
            NonRecursiveFileList(mapsRoot, files);
            auto it = files.begin();
            auto itEnd = files.end();
            for (; it != itEnd; ++it)
            {
                if (it->extension().string() != ".lua")
                {
                    Tools::log << "Map " << *it << " ignored (unknown extension " << it->extension() << ")." << std::endl;
                    continue;
                }
                std::string name = it->stem().string();
                if (!Common::FieldValidator::IsMapName(name))
                {
                    Tools::log << "Invalid map name \"" << name << "\", map ignored." << std::endl;
                    continue;
                }
                auto data = ReadFile(*it);
                conn.CreateQuery("INSERT INTO map (name, lua) VALUES (?, ?);")->
                    Bind(name).Bind(std::string(data.begin(), data.end())).ExecuteNonSelect();
                Tools::log << "Added map \"" << name << "\" (size: " << data.size() << " bytes)." << std::endl;
            }
        }

    }

    bool Create(boost::filesystem::path const& pluginRoot, boost::filesystem::path const& destFile)
    {
        // check de base des chemins
        if (!boost::filesystem::is_directory(pluginRoot))
        {
            Tools::log << "Error: " << pluginRoot << " is not a valid plugin root directory." << pluginRoot.filename() << std::endl;
            return false;
        }
        if (boost::filesystem::is_directory(destFile))
        {
            Tools::log << "Error: " << destFile << " is not a valid plugin destination file." << std::endl;
            return false;
        }

        // supprime le fichier destination si il existe deja
        if (boost::filesystem::exists(destFile))
            boost::filesystem::remove(destFile);

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
            Tools::log << "Error: Failed to read plugin configuration file: " << e.what() << std::endl;
            return false;
        }
        Tools::log << "Packaging plugin \"" << identifier << "\" (fullname: \"" << fullname << "\") from " << pluginRoot << " in " << destFile << "." << std::endl;

        // debut d'utilisation de la base
        Tools::Database::IConnection* conn = 0;
        try
        {
            conn = new Tools::Database::Sqlite::Connection(destFile.string());

            // structure de la base
            conn->CreateQuery("CREATE TABLE plugin (build_hash TEXT, fullname TEXT, identifier TEXT, format_version INTEGER);")->ExecuteNonSelect();
            conn->CreateQuery("CREATE TABLE resource (name TEXT, type TEXT, data_hash TEXT, data BLOB);")->ExecuteNonSelect();
            conn->CreateQuery("CREATE TABLE map (name TEXT, lua TEXT);")->ExecuteNonSelect();
            conn->CreateQuery("CREATE TABLE entity_file (name TEXT, lua TEXT);")->ExecuteNonSelect();
            conn->CreateQuery("CREATE TABLE cube_file (name TEXT, lua TEXT);")->ExecuteNonSelect();

            // remplissage
            FillTablePlugin(*conn, identifier, fullname);
            FillTableResource(pluginRoot, *conn);
            FillTableEntityFile(pluginRoot, *conn);
            FillTableCubeFile(pluginRoot, *conn);
            FillTableMap(pluginRoot, *conn);

            Tools::Delete(conn);
        }
        catch (std::exception& e)
        {
            Tools::log << "Error: " << e.what() << std::endl;
            Tools::Delete(conn);
            return false;
        }

        // success!
        Tools::log << "Plugin \"" << identifier << "\" successfully packaged in " << destFile << "." << std::endl;
        return true;
    }

}}
