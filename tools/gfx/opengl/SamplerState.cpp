#include "tools/precompiled.hpp"

#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/opengl/SamplerState.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    SamplerState::SamplerState(GLRenderer& renderer) :
        _renderer(renderer),
        _mipFilter(TextureFilter::None)
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
        GLCHECK(glSamplerParameteri(this->_id, GL_TEXTURE_MIN_FILTER, GetTextureFilter(this->_minFilter, this->_mipFilter)));
    }

    void SamplerState::SetMagFilter(TextureFilter::Type filter)
    {
        this->_magFilter = filter;
        GLCHECK(glSamplerParameteri(this->_id, GL_TEXTURE_MAG_FILTER, GetTextureFilter(filter)));
    }

    void SamplerState::SetMipFilter(TextureFilter::Type filter)
    {
        this->_mipFilter = filter;
        GLCHECK(glSamplerParameteri(this->_id, GL_TEXTURE_MIN_FILTER, GetTextureFilter(this->_minFilter, this->_mipFilter)));
    }

    void SamplerState::SetMaxAnisotropy(int value)
    {
        GLCHECK(glSamplerParameterf(this->_id, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value));
    }

    void SamplerState::SetMaxLOD(int value)
    {
        GLCHECK(glSamplerParameterf(this->_id, GL_TEXTURE_MAX_LOD, value == 0 ? 1000.0f : (float)value));
    }

    void SamplerState::SetAddressU(TextureAddress::Type mode)
    {
        GLCHECK(glSamplerParameteri(this->_id, GL_TEXTURE_WRAP_S, GetTextureAddress(mode)));
    }

    void SamplerState::SetAddressV(TextureAddress::Type mode)
    {
        GLCHECK(glSamplerParameteri(this->_id, GL_TEXTURE_WRAP_T, GetTextureAddress(mode)));
    }
    void SamplerState::SetAddressW(TextureAddress::Type mode)
    {
        GLCHECK(glSamplerParameteri(this->_id, GL_TEXTURE_WRAP_R, GetTextureAddress(mode)));
    }

    void SamplerState::Bind(Uint32 unit)
    {
        GLCHECK(glBindSampler(unit, this->_id));
    }

}}}
