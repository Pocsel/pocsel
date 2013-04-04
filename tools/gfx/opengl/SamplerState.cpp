#include "tools/precompiled.hpp"

#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/opengl/SamplerState.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    SamplerState::SamplerState(GLRenderer& renderer) :
        _renderer(renderer)
    {
        GLCHECK(glGenSamplers(1, &this->_id));
    }

    SamplerState::~SamplerState()
    {
        GLCHECK(glDeleteSamplers(1, &this->_id));
    }

    void SamplerState::SetMinFilter(TextureFilter::Type filter)
    {
        this->_minFilter = filter;
        GLCHECK(glSamplerParameteri(this->_id, GL_TEXTURE_MIN_FILTER, GetTextureFilter(filter)));
    }

    void SamplerState::SetMagFilter(TextureFilter::Type filter)
    {
        this->_magFilter = filter;
        GLCHECK(glSamplerParameteri(this->_id, GL_TEXTURE_MAG_FILTER, GetTextureFilter(filter)));
    }

    void SamplerState::Bind(Uint32 unit)
    {
        GLCHECK(glBindSampler(unit, this->_id));
    }

}}}
