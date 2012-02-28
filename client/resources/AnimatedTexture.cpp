#include "tools/precompiled.hpp"

#include "common/Resource.hpp"
#include "tools/logger/Logger.hpp"
#include "tools/lua/Interpreter.hpp"
#include "client/game/Game.hpp"
#include "client/resources/ResourceManager.hpp"
#include "client/resources/AnimatedTexture.hpp"

namespace Client { namespace Resources {

    AnimatedTexture::AnimatedTexture(Game::Game& game, Uint32 pluginId, std::string const& description)
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
            this->_frames = &game.GetResourceManager().GetAnimatedTexture(pluginId, globals["texture"].ToString());
            this->_timePerFrame = Uint64(animationTime * 1000000 / this->_frames->size());
        }
        catch (std::exception& e)
        {
            Tools::error << "AnimatedTexture: Can't load description (error: " << e.what() << ").\n";
            throw;
        }
    }

    void AnimatedTexture::Update(Uint64 totalTime)
    {
        if ((totalTime - this->_lastFrame) >= this->_timePerFrame)
        {
           this->_currentFrame = int((this->_currentFrame + 1) % this->_frames->size());
           this->_lastFrame = totalTime;
        }
    }

    void AnimatedTexture::Bind()
    {
        this->GetCurrentTexture().Bind();
    }

    void AnimatedTexture::Unbind()
    {
        this->GetCurrentTexture().Unbind();
    }

}}
