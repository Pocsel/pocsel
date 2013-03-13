#ifndef __TOOLS_PLUGIN_CREATE_CREATE_HPP__
#define __TOOLS_PLUGIN_CREATE_CREATE_HPP__

namespace Tools { namespace PluginCreate {

    class FileConverter
    {
    public:
        std::string type;

        virtual ~FileConverter() {}
        virtual std::vector<char> Convert(std::string const& file) const = 0;

    protected:
        FileConverter(std::string const& type) : type(type) {}
    };

    // Retourne les convertisseurs par défaults
    std::map<std::string, std::unique_ptr<FileConverter>> GetDefaultConverter();

    // fileConverters: <extention, convertisseur>
    bool Create(boost::filesystem::path const& pluginRootDir, boost::filesystem::path const& destFile, std::map<std::string, std::unique_ptr<FileConverter>> const& fileConverters);
    inline bool Create(boost::filesystem::path const& pluginRootDir, boost::filesystem::path const& destFile) { return Create(pluginRootDir, destFile, GetDefaultConverter()); }
}}

#endif
