#ifndef __TOOLS_PLUGIN_CREATE_CREATE_HPP__
#define __TOOLS_PLUGIN_CREATE_CREATE_HPP__

namespace Tools { namespace PluginCreate {

    struct Resource
    {
        std::string type;
        boost::filesystem::path srcFile;
        boost::filesystem::path dstFile;
        bool isTemporaryFile;

        Resource(Resource&& rhs);
        Resource(std::string type, boost::filesystem::path srcFile, boost::filesystem::path dstFile, bool temp);
        ~Resource();
    private:
        Resource(Resource const&);
        Resource& operator = (Resource const&);
    };

    // Retourne les convertisseurs par défaults
    std::map<std::string, std::function<Resource(boost::filesystem::path const&)>> GetDefaultConverter();

    // fileConverters: <extention, convertisseur>
    Resource Convert(boost::filesystem::path const& file, std::map<std::string, std::function<Resource(boost::filesystem::path const&)>> const& fileConverters);
    std::list<Resource> ConvertAllFiles(boost::filesystem::path const& directory, std::map<std::string, std::function<Resource(boost::filesystem::path const&)>> const& fileConverters);

    bool Create(boost::filesystem::path const& pluginRootDir, boost::filesystem::path const& destFile, std::list<Resource> const& resources);
}}

#endif
