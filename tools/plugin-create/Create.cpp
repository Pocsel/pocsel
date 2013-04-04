#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/crc.hpp>
#include <luasel/Luasel.hpp>

#include "tools/plugin-create/Create.hpp"
#include "tools/database/sqlite/Connection.hpp"
#include "common/FieldUtils.hpp"
#include "common/constants.hpp"

#if BOOST_VERSION >= 105000
namespace boost { namespace filesystem {
#else
namespace boost { namespace filesystem3 {
#endif

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

        template<class T>
        std::unique_ptr<T> MakeUnique(T* ptr)
        {
            return std::unique_ptr<T>(ptr);
        }

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

        std::vector<char> ReadFile(std::string const& file)
        {
            std::ifstream tmp(file, std::ios::binary | std::ios::in);
            return std::vector<char>((std::istreambuf_iterator<char>(tmp)), std::istreambuf_iterator<char>());
        }

        std::string HashFile(std::vector<char> const& data)
        {
            boost::crc_32_type result;
            result.process_bytes(data.data(), data.size());
            return Tools::ToString(result.checksum());
        }

        void FillTablePlugin(std::string const& identifier, std::string const& fullname, Tools::Database::IConnection& conn)
        {
            boost::uuids::random_generator buildHashGenerator;
            conn.CreateQuery("INSERT INTO plugin (build_hash, fullname, identifier, format_version) VALUES (?, ?, ?, ?);")->
                Bind(boost::uuids::to_string(buildHashGenerator())).Bind(fullname).Bind(identifier).Bind(Common::PluginFormatVersion).ExecuteNonSelect();
        }

        void FillTableResource(boost::filesystem::path const& pluginRoot, Tools::Database::IConnection& conn, std::list<Resource> const& resources)
        {
            boost::filesystem::path clientRoot(pluginRoot / "client");
            if (!boost::filesystem::is_directory(clientRoot))
            {
                Tools::log << clientRoot << " not found, no client files will be packaged." << std::endl;
                return;
            }

            auto query = conn.CreateQuery("INSERT INTO resource (name, type, data_hash, data) VALUES (?, ?, ?, ?);");
            for (auto const& res: resources)
            {
                auto const& data = ReadFile(res.dstFile);
                std::string const& name = MakeRelative(clientRoot, res.srcFile).generic_string();
                std::string const& type = res.type;
                std::string const& hash = HashFile(data);

                query->Bind(name).Bind(type).Bind(hash).Bind(data.data(), data.size()).ExecuteNonSelect().Reset();
                Tools::log << "Added client file \"" << name << "\" (size: " << data.size() << " bytes, hash: \"" << hash << "\", type: \"" << type << "\")." << std::endl;
            }
        }

        void FillTableServerFile(boost::filesystem::path const& pluginRoot, Tools::Database::IConnection& conn)
        {
            boost::filesystem::path serverRoot(pluginRoot / "server");
            if (!boost::filesystem::is_directory(serverRoot))
            {
                Tools::log << serverRoot << " not found, no server files will be packaged." << std::endl;
                return;
            }

            std::list<boost::filesystem::path> files;
            RecursiveFileList(serverRoot, files);
            auto it = files.begin();
            auto itEnd = files.end();
            auto query = conn.CreateQuery("INSERT INTO server_file (name, lua) VALUES (?, ?);");
            for (; it != itEnd; ++it)
            {
                if (it->extension().string() != ".lua")
                {
                    Tools::log << "Server file " << *it << " ignored (unknow extension " << it->extension() << ")." << std::endl;
                    continue;
                }
                auto data = ReadFile(*it);
                std::string name = MakeRelative(serverRoot, *it).generic_string();
                query->Bind(name).Bind(std::string(data.begin(), data.end())).ExecuteNonSelect().Reset();
                Tools::log << "Added server file \"" << name << "\" (size: " << data.size() << " bytes)." << std::endl;
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
            auto query = conn.CreateQuery("INSERT INTO map (name, lua) VALUES (?, ?);");
            for (; it != itEnd; ++it)
            {
                if (it->extension().string() != ".lua")
                {
                    Tools::log << "Map " << *it << " ignored (unknown extension " << it->extension() << ")." << std::endl;
                    continue;
                }
                std::string name = it->stem().string();
                if (!Common::FieldUtils::IsMapName(name))
                {
                    Tools::log << "Invalid map name \"" << name << "\", map ignored (must be alphamumeric, 20 characters max.)." << std::endl;
                    continue;
                }
                auto data = ReadFile(*it);
                query->Bind(name).Bind(std::string(data.begin(), data.end())).ExecuteNonSelect().Reset();
                Tools::log << "Added map \"" << name << "\" (size: " << data.size() << " bytes)." << std::endl;
            }
        }

        void CreatePluginTables(Tools::Database::IConnection& conn)
        {
            conn.CreateQuery("CREATE TABLE plugin (build_hash TEXT, fullname TEXT, identifier TEXT, format_version INTEGER);")->ExecuteNonSelect();
            conn.CreateQuery("CREATE TABLE resource (name TEXT, type TEXT, data_hash TEXT, data BLOB);")->ExecuteNonSelect();
            conn.CreateQuery("CREATE TABLE script (name TEXT, type TEXT);")->ExecuteNonSelect();
            conn.CreateQuery("CREATE TABLE map (name TEXT, lua TEXT);")->ExecuteNonSelect();
            conn.CreateQuery("CREATE TABLE server_file (name TEXT, lua TEXT);")->ExecuteNonSelect();
        }

        void ReadPluginConfiguration(boost::filesystem::path const& conf, std::string& identifier, std::string& fullname)
        {
            Luasel::Interpreter interpreter;
            interpreter.DoFile(conf.string());
            Luasel::Ref ref(interpreter.GetState());
            ref = interpreter.Globals()["identifier"];
            if (ref.Exists())
                identifier = ref.CheckString("identifier must be a string");
            else
                throw std::runtime_error("configuration string identifier not found");
            ref = interpreter.Globals()["fullname"];
            if (ref.Exists())
                fullname = ref.CheckString("fullname must be a string");
            else
                fullname = identifier;
        }

    }

    Resource::Resource(Resource&& rhs) :
        type(std::move(rhs.type)),
        srcFile(std::move(rhs.srcFile)),
        dstFile(std::move(rhs.dstFile)),
        isTemporaryFile(rhs.isTemporaryFile)
    {
        rhs.isTemporaryFile = false;
    }

    Resource::Resource(std::string type, boost::filesystem::path srcFile, boost::filesystem::path dstFile, bool temp) :
        type(std::move(type)),
        srcFile(std::move(srcFile)),
        dstFile(std::move(dstFile)),
        isTemporaryFile(temp)
    {
    }

    Resource::~Resource()
    {
        if (this->isTemporaryFile)
            boost::filesystem::remove(this->dstFile);
    }

    std::map<std::string, std::function<Resource(boost::filesystem::path const&)>> GetDefaultConverter()
    {
        auto func = [](std::string type) -> std::function<Resource(boost::filesystem::path const&)>
        {
            return [=](boost::filesystem::path const& file)
            {
                return Resource(type, file, file, false);
            };
        };
        std::map<std::string, std::function<Resource(boost::filesystem::path const&)>> converters;
        converters[".lua"] = func("lua");
        converters[".png"] = func("image");
        converters[".fxc"] = func("effect");
        converters[".mqm"] = func("model");
        return converters;
    }

    Resource Convert(boost::filesystem::path const& file, std::map<std::string, std::function<Resource(boost::filesystem::path const&)>> const& fileConverters)
    {

        auto itType = fileConverters.find(file.extension().string());
        if (itType == fileConverters.end())
            throw std::runtime_error("unknown extension " + file.extension().generic_string());

        return itType->second(file.generic_string());
    }

    std::list<Resource> ConvertAllFiles(boost::filesystem::path const& directory, std::map<std::string, std::function<Resource(boost::filesystem::path const&)>> const& fileConverters)
    {
        std::list<boost::filesystem::path> files;
        RecursiveFileList(directory, files);

        std::list<Resource> resources;
        for (auto const& file: files)
        {
            try
            {
                resources.push_back(Convert(file, fileConverters));
            }
            catch (std::exception& ex)
            {
                Tools::log << "Client file " << file << " ignored (" << ex.what() << ")." << std::endl;
            }
        }
        return resources;
    }

    bool Create(boost::filesystem::path const& pluginRoot, boost::filesystem::path const& destFile, std::list<Resource> const& resources)
    {
        // check de base des chemins
        if (!boost::filesystem::is_directory(pluginRoot))
        {
            Tools::log << "Error: " << pluginRoot << " is not a valid plugin root directory." << std::endl;
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
            ReadPluginConfiguration(pluginRoot / "plugin.lua", identifier, fullname);
        }
        catch (std::exception& e)
        {
            Tools::log << "Error: Failed to read plugin configuration file: " << e.what() << std::endl;
            return false;
        }
        if (!Common::FieldUtils::IsPluginIdentifier(identifier))
        {
            Tools::log << "Error: Invalid plugin identifier \"" << identifier << "\" (must be alphanumeric, 20 characters max.)." << std::endl;
            return false;
        }
        if (!Common::FieldUtils::IsPluginFullname(fullname))
        {
            Tools::log << "Error: Invalid plugin fullname \"" << fullname << "\" (100 characters max.)." << std::endl;
            return false;
        }
        Tools::log << "Packaging plugin \"" << identifier << "\" (fullname: \"" << fullname << "\") from " << pluginRoot << " in " << destFile << "." << std::endl;

        // debut d'utilisation de la base
        Tools::Database::IConnection* conn = 0;
        try
        {
            conn = new Tools::Database::Sqlite::Connection(destFile.string());
            CreatePluginTables(*conn);
            conn->BeginTransaction();

            // remplissage
            FillTablePlugin(identifier, fullname, *conn);
            FillTableResource(pluginRoot, *conn, resources);
            FillTableServerFile(pluginRoot, *conn);
            FillTableMap(pluginRoot, *conn);

            Tools::log << "Finishing writing to database..." << std::endl;
            conn->EndTransaction();
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
