#include "tools/precompiled.hpp"

#include "common/Resource.hpp"
#include "tools/logger/Logger.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/renderers/utils/texture/AnimatedTexture.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Texture {

    AnimatedTexture::AnimatedTexture(std::vector<ITexture2D*> const& frames, Uint64 timePerFrame, bool animated) :
        _frames(&frames),
        _timePerFrame(timePerFrame),
        _animated(animated),
        _currentFrame(0),
        _lastFrame(0)
    {
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

}}}}
