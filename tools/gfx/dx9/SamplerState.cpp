#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/gfx/dx9/directx.hpp"
#include "tools/gfx/dx9/SamplerState.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    SamplerState::SamplerState(DX9Renderer& renderer) :
        _renderer(renderer)
    {
    }

    SamplerState::~SamplerState()
    {
    }

    void SamplerState::SetMinFilter(TextureFilter::Type filter)
    {
        this->_minFilter = filter;
    }

    void SamplerState::SetMagFilter(TextureFilter::Type filter)
    {
        this->_magFilter = filter;
    }

    void SamplerState::Bind(Uint32 unit)
    {
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_MINFILTER, GetTextureFilter(this->_minFilter));
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_MAGFILTER, GetTextureFilter(this->_magFilter));
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_MIPFILTER, GetTextureFilter(this->_minFilter));
    }

}}}

#endif