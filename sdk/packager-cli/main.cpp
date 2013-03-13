#include <boost/cstdlib.hpp>
#include "tools/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include "sdk/packager-cli/ProgramInfo.hpp"
#include "tools/plugin-create/Create.hpp"

namespace {

    struct RAII
    {
        std::function<void()> dtor;
        RAII(std::function<void()> dtor) : dtor(dtor) {}
        ~RAII() { dtor(); }
    };

    class FxCompiler : public Tools::PluginCreate::FileConverter
    {
    public:
        FxCompiler() : FileConverter("effect") {}
        virtual std::vector<char> Convert(std::string const& file) const
        {
            auto tempFile = boost::filesystem::unique_path();
            RAII removeFile([&]() { boost::filesystem::remove(tempFile); });

            int retval = system(("compiler-shader " + file + " " + tempFile.generic_string()).c_str());
            if (retval != 0)
                throw std::runtime_error("Can't convert \"" + file +"\"");

            boost::filesystem::ifstream tmp(tempFile, std::ios::binary | std::ios::in);
            return std::vector<char>((std::istreambuf_iterator<char>(tmp)), std::istreambuf_iterator<char>());
        }
    };

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
    auto converters = Tools::PluginCreate::GetDefaultConverter();
    //converters[".fx"] = std::unique_ptr<FxCompiler>(new FxCompiler()); // TODO
    converters[".fx"] = std::move(converters[".fxc"]);
    converters.erase(".fxc");
    return Tools::PluginCreate::Create(av[1], av[2], converters) ? boost::exit_success : boost::exit_failure;
}
