#include "client2/precompiled.hpp"

#include "client2/Client.hpp"
#include "client2/Settings.hpp"
#include "client2/network/Network.hpp"
#include "client2/network/PacketCreator.hpp"
#include "client2/resources/LocalResourceManager.hpp"
#include "client2/window/Window.hpp"

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/Font.hpp"

namespace Client { namespace Resources {

    LocalResourceManager::LocalResourceManager(Client& client)
        : _client(client),
        _renderer(client.GetWindow().GetRenderer())
    {
        this->_InitErrorTexture();
    }

    LocalResourceManager::~LocalResourceManager()
    {
        std::for_each(this->_textures.begin(), this->_textures.end(),
            [](std::pair<std::string, Tools::Renderers::ITexture2D*> const& pair)
            {
                delete pair.second;
            });
        std::for_each(this->_shaders.begin(), this->_shaders.end(),
            [](std::pair<std::string, Tools::Renderers::IShaderProgram*> const& pair)
            {
                delete pair.second;
            });
    }

    Tools::Renderers::Utils::Font& LocalResourceManager::GetFont(std::string const& path, Uint32 size)
    {
        std::string fontId =  path + Tools::ToString(size);
        auto it = this->_fonts.find(fontId);
        if (it == this->_fonts.end())
        {
            try
            {
                auto fontPath = (this->_client.GetSettings().confDir / "fonts" / path).string();
                auto font = new Tools::Renderers::Utils::Font(this->_renderer, fontPath, size);
                this->_fonts[fontId] = font;
                return *font;
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load font \"" << path << "\", details: " << ex.what() << "\n";
                throw;
            }
        }
        else
            return *it->second;
    }

    Tools::Renderers::ITexture2D& LocalResourceManager::GetTexture2D(std::string const& path)
    {
        auto it = this->_textures.find(path);
        if (it == this->_textures.end())
        {
            Tools::Renderers::ITexture2D* texture = 0;
            try
            {
                texture = this->_renderer.CreateTexture2D((this->_client.GetSettings().confDir / "textures" / path).string()).release();
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load texture \"" << path << "\", details: " << ex.what() << "\n";
                texture = this->_textures["__error__"];
            }
            this->_textures[path] = texture;
            return *texture;
        }
        else
            return *it->second;
    }

    Tools::Renderers::IShaderProgram& LocalResourceManager::GetShader(std::string const& path)
    {
        auto it = this->_shaders.find(path);
        if (it == this->_shaders.end())
        {
            try
            {
                std::ifstream tmp((this->_client.GetSettings().confDir / "shaders" / path).string());
                std::string shaderString((std::istreambuf_iterator<char>(tmp)), std::istreambuf_iterator<char>());
                auto shader = this->_renderer.CreateProgram(shaderString).release();
                this->_shaders[path] = shader;
                return *shader;
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load shader \"" << path << "\", details: " << ex.what() << "\n";
                throw;
            }
        }
        else
            return *it->second;
    }

    void LocalResourceManager::_InitErrorTexture()
    {
        unsigned char toto[] = {
            0, 0, 0, 255,
            255, 0, 255, 255,
            255, 0, 255, 255,
            0, 0, 0, 255
        };
        this->_textures["__error__"] = this->_renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Rgba8, 100312, toto, Tools::Vector2u(2, 2)).release();
    }

}}