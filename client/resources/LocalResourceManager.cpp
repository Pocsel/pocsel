#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/Settings.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/resources/LocalResourceManager.hpp"
#include "client/resources/Md5Model.hpp"
#include "client/resources/Md5Animation.hpp"
#include "client/window/Window.hpp"

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
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_shaders.begin(), ite = this->_shaders.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_fonts.begin(), ite = this->_fonts.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_models.begin(), ite = this->_models.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_animations.begin(), ite = this->_animations.end(); it != ite; ++it)
            Tools::Delete(it->second);
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
                Tools::error << "Can't load local font \"" << path << "\", details: " << ex.what() << "\n";
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
                Tools::error << "Can't load local texture \"" << path << "\", details: " << ex.what() << "\n";
                texture = this->_textures["__error__"];
            }
            if (texture == this->_textures["__error__"])
                this->_textures[path] = 0;
            else
                this->_textures[path] = texture;
            return *texture;
        }
        else if (it->second != 0)
            return *it->second;
        return *this->_textures["__error__"];
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
                Tools::error << "Can't load local shader \"" << path << "\", details: " << ex.what() << "\n";
                throw;
            }
        }
        else
            return *it->second;
    }

    Md5Model const& LocalResourceManager::GetMd5Model(std::string const& path)
    {
        auto it = this->_models.find(path);
        if (it == this->_models.end())
        {
            try
            {
                boost::filesystem::path texturesPath = path;
                boost::filesystem::path modelPath = this->_client.GetSettings().confDir / "models" / path;
                modelPath.replace_extension(".iqm");
                Md5Model* model = new Md5Model(modelPath, texturesPath, *this);
                this->_models[path] = model;
                return *model;
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load Md5Model \"" << path << "\", details: " << ex.what() << "\n";
                throw;
            }
        }
        else
            return *it->second;
    }

    Md5Animation const& LocalResourceManager::GetMd5Animation(std::string const& path)
    {
        auto it = this->_animations.find(path);
        if (it == this->_animations.end())
        {
            try
            {
                boost::filesystem::path animPath = this->_client.GetSettings().confDir / "models" / path;
                animPath.replace_extension(".md5anim");
                Md5Animation* anim = new Md5Animation(animPath);
                this->_animations[path] = anim;
                return *anim;
            }
            catch (std::exception& ex)
            {
                Tools::error << "Can't load Md5Anim \"" << path << "\", details: " << ex.what() << "\n";
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
        this->_textures["__error__"] = this->_renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Rgba8, 16, toto, glm::uvec2(2, 2)).release();
    }

}}
