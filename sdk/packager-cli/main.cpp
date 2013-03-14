#include <boost/cstdlib.hpp>
#include "tools/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include "sdk/packager-cli/ProgramInfo.hpp"
#include "tools/plugin-create/Create.hpp"
#include "tools/SystemUtils.hpp"

namespace {

    Tools::PluginCreate::Resource CompileEffect(std::string const& compilerExe, boost::filesystem::path const& file)
    {
        auto tempFile = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::string command(compilerExe + " \"" + file.generic_string() + "\" \"" + tempFile.generic_string() + "\"");
        Tools::log << command << std::endl;
        int retval = system(command.c_str());
        if (retval != 0)
            throw std::runtime_error("Can't convert \"" + file.generic_string() +"\"");

        return Tools::PluginCreate::Resource("effect", file, tempFile, true);
    }

    void Usage()
    {
        Tools::log << "Usage: " PROGRAM_NAME " PLUGIN_ROOT_DIRECTORY DESTINATION_FILE" << std::endl;
        exit(boost::exit_failure);
    }
}

int main(int ac, char** av)
{
    if (ac != 3)
        Usage();

    std::string compilerExe;
    boost::filesystem::path path(Tools::GetCurrentExecutablePath());
    path = path.parent_path();
    boost::filesystem::directory_iterator it(path);
    boost::filesystem::directory_iterator itEnd;
    for (; it != itEnd; ++it)
    {
        auto const& filename = it->path().filename().generic_string();
        if (filename == "compiler-shader" || filename == "compiler-shader.exe")
            compilerExe = it->path().generic_string();
    }

    auto converters = Tools::PluginCreate::GetDefaultConverter();
    converters[".fx"] = [&](boost::filesystem::path const& file) { return CompileEffect(compilerExe, file); };
    //converters[".fx"] = converters[".fxc"];

    // Convert resources
    auto const& resources = Tools::PluginCreate::ConvertAllFiles(boost::filesystem::path(av[1]), converters);

    return Tools::PluginCreate::Create(av[1], av[2], resources) ? boost::exit_success : boost::exit_failure;
}
