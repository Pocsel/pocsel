#ifndef __CLIENT_RESOURCES_LOCALRESOURCEMANAGER_HPP__
#define __CLIENT_RESOURCES_LOCALRESOURCEMANAGER_HPP__

namespace Tools {
    class IRenderer;
    namespace Renderers {
        namespace Utils {
            class Font;
        }
        class IShaderProgram;
        class ITexture2D;
    }
}
namespace Client {
    class Client;
}

namespace Client { namespace Resources {

    class LocalResourceManager
        : private boost::noncopyable
    {
        private:
            Client& _client;
            Tools::IRenderer& _renderer;
            std::map<std::string, Tools::Renderers::Utils::Font*> _fonts;
            std::map<std::string, Tools::Renderers::IShaderProgram*> _shaders;
            std::map<std::string, Tools::Renderers::ITexture2D*> _textures;

        public:
            LocalResourceManager(Client& client);
            ~LocalResourceManager();

            Tools::Renderers::Utils::Font& GetFont(std::string const& path, Uint32 size);
            Tools::Renderers::ITexture2D& GetTexture2D(std::string const& path);
            Tools::Renderers::IShaderProgram& GetShader(std::string const& path);

        private:
            void _InitErrorTexture();
    };

}}

#endif