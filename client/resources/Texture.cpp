#include "tools/precompiled.hpp"
#include <IL/il.h>
#include <IL/ilu.h>

#include "common/Resource.hpp"
#include "tools/logger/Logger.hpp"
#include "tools/lua/Interpreter.hpp"
#include "client/game/Game.hpp"
#include "client/resources/ResourceManager.hpp"
#include "client/resources/Texture.hpp"

namespace Client { namespace Resources {

    Texture::Texture(Game::Game& game, Uint32 pluginId, std::string const& description)
        : _currentFrame(0),
        _lastFrame(0)
    {
        try
        {
            Tools::Lua::Interpreter i;
            i.DoString(description);
            auto const& globals = i.Globals();
            this->_animated = globals["animated"].ToBoolean();
            auto animationTime = globals["animationTime"].ToNumber();

            auto res = game.GetResourceManager().GetResource(pluginId, globals["texture"].ToString());
            this->_InitializeFrames(game.GetRenderer(), res->data, res->size);

            this->_timePerFrame = Uint64(animationTime * 0.000001 / this->_frames.size());
        }
        catch (std::exception& e)
        {
            Tools::error << "AnimatedTexture: Can't load description (error: " << e.what() << ").\n";
            throw;
        }
    }

    void Texture::Update(Uint64 totalTime)
    {
        if (totalTime - this->_lastFrame >= this->_timePerFrame)
            this->_currentFrame = int((this->_currentFrame + 1) % this->_frames.size());
    }

    void Texture::Bind()
    {
        this->GetCurrentTexture().Bind();
    }

    void Texture::Unbind()
    {
        this->GetCurrentTexture().Unbind();
    }

    void Texture::_InitializeFrames(Tools::IRenderer& renderer, void const* data, std::size_t dataSize)
    {
        ilDisable(IL_BLIT_BLEND);

        ILuint id = ilGenImage();
        ilBindImage(id);

        if (!ilLoadL(IL_PNG, data, (ILuint)dataSize) && !ilLoadL(IL_TYPE_UNKNOWN, data, (ILuint)dataSize))
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: Can't load data");
        }
        int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
        if (bytesPerPixel != 3 && bytesPerPixel != 4)
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: A texture must be 24 or 32 bits per pixels.");
        }

        auto size = Tools::Vector2u((ILuint)ilGetInteger(IL_IMAGE_WIDTH), (ILuint)ilGetInteger(IL_IMAGE_HEIGHT));
        if (size.w % size.h != 0)
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: height must be a multiple of width.");
        }

        auto frameSize = Tools::Vector2u(size.w);
        auto pixmap = new Tools::Color4<Uint8>[frameSize.w * frameSize.h];
        this->_frames.resize(size.h / size.w);
        for (unsigned int y = 0, i = 0; y < size.h; y += size.w, ++i)
        {
            ilCopyPixels(0, y, 0, frameSize.w, frameSize.h, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);
            this->_frames[i] = renderer.CreateTexture2D(Tools::Renderers::PixelFormat::Rgba8, frameSize.w * frameSize.h, pixmap, frameSize, 0);
        }
        delete pixmap;
        ilBindImage(0);
        ilDeleteImage(id);
    }

}}
