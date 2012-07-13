#include "client/precompiled.hpp"

#include <IL/il.h>

#include "tools/IRenderer.hpp"
//#include "tools/renderers/utils/texture/TextureAtlas.hpp"
#include "tools/models/ErrorModel.hpp"
#include "tools/models/MqmModel.hpp"
#include "tools/renderers/utils/material/LuaMaterial.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"
#include "tools/renderers/utils/texture/AnimatedTexture.hpp"
#include "tools/renderers/utils/texture/Texture.hpp"
#include "tools/window/Window.hpp"
#include "tools/lua/Interpreter.hpp"

#include "common/FieldUtils.hpp"
#include "common/Resource.hpp"

#include "client/Client.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/resources/CacheDatabaseProxy.hpp"
#include "client/resources/ResourceDownloader.hpp"
#include "client/resources/ResourceManager.hpp"
#include "client/Settings.hpp"

#define data shaderFxError
#include "client/resources/ShaderError.fx.hpp"
#undef data
#define data materialLuaError
#include "client/resources/MaterialError.lua.hpp"
#undef data

namespace {

    inline void InitializeFrames(Tools::IRenderer& renderer, void const* data, std::size_t dataSize, std::vector<Tools::Renderers::ITexture2D*>& frames)
    {
        ilDisable(IL_BLIT_BLEND);

        ILuint id = ilGenImage();
        ilBindImage(id);

        if (!ilLoadL(IL_PNG, data, (ILuint)dataSize) && !ilLoadL(IL_TYPE_UNKNOWN, data, (ILuint)dataSize))
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("Texture: Can't load data");
        }
        int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
        if (bytesPerPixel != 3 && bytesPerPixel != 4)
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("Texture: A texture must be 24 or 32 bits per pixels.");
        }

        auto size = glm::uvec2((ILuint)ilGetInteger(IL_IMAGE_WIDTH), (ILuint)ilGetInteger(IL_IMAGE_HEIGHT));
        if (size.y % size.x != 0)
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("Texture: height must be a multiple of width.");
        }

        auto frameSize = glm::uvec2(size.x);
        auto pixmap = new glm::u8vec4[frameSize.x * frameSize.y];
        frames.resize(size.y / size.x);
        for (unsigned int y = 0, i = 0; y < size.y; y += size.x, ++i)
        {
            ilCopyPixels(0, y, 0, frameSize.x, frameSize.y, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);
            frames[i] = renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Rgba8, frameSize.x * frameSize.y * 4, pixmap, frameSize, 0).release();
        }
        Tools::Delete(pixmap);
        ilBindImage(0);
        ilDeleteImage(id);
    }

}

namespace Client { namespace Resources {

    ResourceManager::ResourceManager(Game::Game& game, std::string const& host, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, std::string const& worldBuildHash) :
        _game(game),
        _database(game.GetClient().GetSettings().cacheDir, host, worldIdentifier, worldName, worldVersion, worldBuildHash),
        _downloader(_database, game.GetClient().GetNetwork()),
        _renderer(game.GetClient().GetWindow().GetRenderer())
    {
        Tools::log << "Cache version " << this->_database.GetCacheVersion() << ", asking for resources...\n";
        game.GetClient().GetNetwork().SendPacket(Network::PacketCreator::GetNeededResourceIds(this->_database.GetCacheVersion()));
    }

    ResourceManager::~ResourceManager()
    {
        Tools::Renderers::ITexture2D* errTex = 0;
        if (this->_rawTextures.find(0) != this->_rawTextures.end())
            errTex = this->_rawTextures[0];
        for (auto it = this->_rawTextures.begin(), ite = this->_rawTextures.end(); it != ite; ++it)
            if (it->second != errTex)
                Tools::Delete(it->second);
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_animatedTextureFrames.begin(), ite = this->_animatedTextureFrames.end(); it != ite; ++it)
            for (unsigned int i = 0; i < it->second.size(); ++i)
                if (it->second[i] != errTex)
                    Tools::Delete(it->second[i]);
        Tools::Delete(errTex);

        Tools::Models::MqmModel* errModel = 0;
        if (this->_models.count(0))
            errModel = this->_models[0];
        for (auto it = this->_models.begin(), ite = this->_models.end(); it != ite; ++it)
            if (it->second != errModel)
                Tools::Delete(it->second);
        Tools::Delete(errModel);

        for (auto it = this->_shaders.begin(), ite = this->_shaders.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture> ResourceManager::GetTexture(Uint32 id)
    {
        auto it = this->_textures.find(id);
        if (it == this->_textures.end())
        {
            auto res = this->_database.GetResource(id);

            std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture> texture;

            if (res != 0)
            {
                if (res->type == "lua")
                {
                    try
                    {
                        auto description = this->GetScript(id);

                        Tools::Lua::Interpreter i;
                        i.DoString(description);
                        auto const& globals = i.Globals();
                        bool animated = globals["animated"].ToBoolean();
                        auto animationTime = globals["animationTime"].ToNumber();
                        auto framesTexture = globals["texture"].ToString();
                        auto& frames = this->_animatedTextureFrames[this->GetResourceId(framesTexture)];
                        auto framesRes = this->GetResource(framesTexture);
                        InitializeFrames(this->_renderer, framesRes->data, framesRes->size, frames);
                        auto timePerFrame = Uint64(animationTime * 1000000 / frames.size());

                        texture.reset(new Tools::Renderers::Utils::Texture::AnimatedTexture(frames, timePerFrame, animated));
                    }
                    catch (std::exception& e)
                    {
                        Tools::error << "Texture: Can't load texture (error: " << e.what() << ").\n";
                    }
                }
                else if (res->type == "image")
                {
                    auto& img = this->_rawTextures[id];
                    img = this->_renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Png, res->size, res->data).release();
                    texture.reset(new Tools::Renderers::Utils::Texture::Texture(*img));
                }
                else
                    throw std::runtime_error("Bad resource type (remove your cache)");

                this->_textures[id] = texture.release();
                return std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture>(this->_textures[id]->Clone());
            }

            if (this->_rawTextures.find(0) == this->_rawTextures.end())
                this->_InitErrorTexture();
            texture.reset(new Tools::Renderers::Utils::Texture::Texture(*(this->_rawTextures[0])));
            this->_textures[id] = texture.release();
            return std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture>(this->_textures[id]->Clone());
        }
        return std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture>(it->second->Clone());
    }

    Tools::Models::MqmModel const& ResourceManager::GetMqmModel(Uint32 id)
    {
        if (id == 0)
        {
            if (this->_models.count(0) == 0)
                this->_InitErrorModel();
            return *this->_models[0];
        }
        auto it = this->_models.find(id);
        if (it == this->_models.end())
        {
            auto& model = this->_models[id];

            auto res = this->_database.GetResource(id);

            if (res != 0)
            {
                try
                {
                    std::vector<char> data(res->size);
                    std::memcpy(data.data(), res->data, res->size);
                    model = new Tools::Models::MqmModel(data, this->_renderer);
                }
                catch (std::exception& ex)
                {
                    Tools::error << "Can't load MqmModel (" << ex.what() << ")\n";
                }
            }

            if (model == 0)
            {
                if (this->_models.count(0) == 0)
                    this->_InitErrorModel();
                model = this->_models.find(0)->second;
            }
            return *model;
        }
        return *it->second;
    }

    Tools::Renderers::IShaderProgram& ResourceManager::GetShader(Uint32 id)
    {
        auto it = this->_shaders.find(id);
        if (it == this->_shaders.end())
        {
            auto res = this->_database.GetResource(id);
            if (res != 0)
            {
                Tools::Renderers::IShaderProgram* s = this->_renderer.CreateProgram(std::string((char const*)res->data, res->size)).release();
                this->_shaders[id] = s;
                return *s;
            }
            else
            {
                it = this->_shaders.find(0);
                if (it == this->_shaders.end())
                {
                    this->_InitErrorShader();
                    it = this->_shaders.find(0);
                }
            }
        }
        return *it->second;
    }

    std::string ResourceManager::GetScript(Uint32 id)
    {
        auto it = this->_scripts.find(id);
        if (it == this->_scripts.end())
        {
            auto res = this->_database.GetResource(id);
            if (res != 0)
            {
                std::string str((char const*)res->data, res->size);
                this->_scripts[id] = str;
                return str;
            }
            else
                throw std::runtime_error("Bad id (remove your cache)");
        }
        else
            return it->second;
    }

    std::unique_ptr<Common::Resource> ResourceManager::GetResource(Uint32 id)
    {
        return this->_database.GetResource(id);
    }

    std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture> ResourceManager::GetTexture(std::string const& name)
    {
        return this->GetTexture(this->_database.GetResourceId(name));
    }

    Tools::Models::MqmModel const& ResourceManager::GetMqmModel(std::string const& name)
    {
        return this->GetMqmModel(this->_database.GetResourceId(name));
    }

    Tools::Renderers::IShaderProgram& ResourceManager::GetShader(std::string const& name)
    {
        return this->GetShader(this->_database.GetResourceId(name));
    }

    std::string ResourceManager::GetScript(std::string const& name)
    {
        return this->GetScript(this->_database.GetResourceId(name));
    }

    std::unique_ptr<Common::Resource> ResourceManager::GetResource(std::string const& name)
    {
        return this->_database.GetResource(this->_database.GetResourceId(name));
    }

    //Effect& ResourceManager::GetEffect(std::string const& name)
    //{
    //    auto it = this->_effects.find(name);
    //    if (it == this->_effects.end())
    //        throw std::runtime_error("Effect \"" + name + "\" not found");
    //    return *it->second;
    //}

    std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial> ResourceManager::GetMaterial(std::string const& name)
    {
        auto it = this->_materials.find(name);
        if (it == this->_materials.end())
        {
            it = this->_materials.find("error");
            if (it == this->_materials.end())
            {
                this->_InitErrorMaterial();
                it = this->_materials.find("error");
            }
        }
        return std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial>(new Tools::Renderers::Utils::Material::LuaMaterial(*it->second));
    }

    //std::unique_ptr<Tools::Renderers::Utils::TextureAtlas> ResourceManager::CreateTextureAtlas(std::list<Uint32> const& textureIds)
    //{
    //    std::list<std::unique_ptr<Common::Resource>> resources;
    //    std::map<Uint32, std::pair<std::size_t, void const*>> textures;
    //    for (auto it = textureIds.begin(), ite = textureIds.end(); it != ite; ++it)
    //    {
    //        auto res = this->_database.GetResource(*it);
    //        if (res == 0)
    //            textures[*it] = std::pair<std::size_t, void const*>(0, (void const*)0);
    //        else
    //            textures[*it] = std::pair<std::size_t, void const*>(res->size, res->data);
    //        resources.push_back(std::move(res));
    //    }
    //    return std::unique_ptr<Tools::Renderers::Utils::TextureAtlas>(new Tools::Renderers::Utils::TextureAtlas(this->_renderer, textures));
    //}

    void ResourceManager::BuildResourceIndex()
    {
        auto const& resources = this->_database.GetAllResources("%");
        for (auto it = resources.begin(), ite = resources.end(); it != ite; ++it)
            this->_resourceIds[(*it)->name] = (*it)->id;
    }

    void ResourceManager::_InitErrorModel()
    {
        this->_models[0] = Tools::Models::ErrorModel(
                //std::bind(&LocalResourceManager::GetTexture2D, this, std::placeholders::_1),
                this->_renderer
                ).release();
    }

    void ResourceManager::_InitErrorTexture()
    {
        static const unsigned char toto[] = {
            0, 0, 0, 255,
            255, 0, 255, 255,
            255, 0, 255, 255,
            0, 0, 0, 255
        };
        this->_rawTextures[0] = this->_renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Rgba8, sizeof(toto), toto, glm::uvec2(2, 2)).release();
    }

    void ResourceManager::_InitErrorShader()
    {
        this->_shaders[0] = this->_renderer.CreateProgram(std::string((char const*)shaderFxError, sizeof(shaderFxError))).release();
    }

    void ResourceManager::_InitErrorMaterial()
    {
        auto& interpreter = this->_game.GetInterpreter();
        auto prototype = interpreter.LoadString(std::string((char const*)materialLuaError, sizeof(materialLuaError)))();
        auto material = std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial>(new Tools::Renderers::Utils::Material::LuaMaterial(
            this->_renderer,
            prototype,
            std::bind(static_cast<Tools::Renderers::IShaderProgram&(ResourceManager::*)(std::string const&)>(&ResourceManager::GetShader), this, std::placeholders::_1),
            std::bind(static_cast<std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture>(ResourceManager::*)(std::string const&)>(&ResourceManager::GetTexture), this, std::placeholders::_1)));
        this->_materials.insert(std::make_pair("error", std::move(material)));
    }

    void ResourceManager::RegisterLuaFunctions()
    {
        auto clientNs = this->_game.GetInterpreter().Globals().GetTable("Client");
        auto materialNs = clientNs.GetTable("Material");
        materialNs.Set("Register", this->_game.GetInterpreter().MakeFunction(std::bind(&ResourceManager::_ApiRegisterMaterial, this, std::placeholders::_1)));
        //Tools::Renderers::Utils::Material::LuaMaterial::LoadLuaTypes(this->_game.GetInterpreter(), );
    }

    void ResourceManager::_ApiRegisterMaterial(Tools::Lua::CallHelper& helper)
    {
        auto const& pluginName = this->_game.GetEngine().GetRunningPluginName();
        if (pluginName == "")
            throw std::runtime_error("Client.Material.Register: Could not determine currently running plugin, aborting registration");
        auto prototype = helper.PopArg();
        auto const& materialName = prototype["materialName"].CheckString("missing materialName");
        auto material = std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial>(new Tools::Renderers::Utils::Material::LuaMaterial(
            this->_renderer,
            prototype,
            std::bind(static_cast<Tools::Renderers::IShaderProgram&(ResourceManager::*)(std::string const&)>(&ResourceManager::GetShader), this, std::placeholders::_1),
            std::bind(static_cast<std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture>(ResourceManager::*)(std::string const&)>(&ResourceManager::GetTexture), this, std::placeholders::_1)));
        this->_materials.insert(std::make_pair(Common::FieldUtils::GetResourceName(pluginName, materialName), std::move(material)));

        Tools::log << "Material \"" << materialName << "\" registered (plugin: " << pluginName << ")." << std::endl;
    }
}}
