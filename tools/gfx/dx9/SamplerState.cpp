#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/gfx/dx9/directx.hpp"
#include "tools/gfx/dx9/SamplerState.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    SamplerState::SamplerState(DX9Renderer& renderer) :
        _renderer(renderer),
        _mipFilter(TextureFilter::None),
        _minFilter(TextureFilter::Point),
        _magFilter(TextureFilter::Point),
        _maxAnisotropy(4),
        _maxLOD(0),
        _addressU(TextureAddress::Wrap),
        _addressV(TextureAddress::Wrap),
        _addressW(TextureAddress::Wrap)
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

    void SamplerState::SetMipFilter(TextureFilter::Type filter)
    {
        this->_mipFilter = filter;
    }

    void SamplerState::SetMaxAnisotropy(int value)
    {
        this->_maxAnisotropy = value;
    }

    void SamplerState::SetMaxLOD(int value)
    {
        this->_maxLOD = value;
    }

    void SamplerState::SetAddressU(TextureAddress::Type mode)
    {
        this->_addressU = mode;
    }

    void SamplerState::SetAddressV(TextureAddress::Type mode)
    {
        this->_addressV = mode;
    }

    void SamplerState::SetAddressW(TextureAddress::Type mode)
    {
        this->_addressW = mode;
    }

    void SamplerState::Bind(Uint32 unit)
    {
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_MINFILTER, GetTextureFilter(this->_minFilter));
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_MAGFILTER, GetTextureFilter(this->_magFilter));
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_MIPFILTER, GetTextureFilter(this->_mipFilter));
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_MAXANISOTROPY, this->_maxAnisotropy);
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_MAXMIPLEVEL, this->_maxLOD);
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_ADDRESSU, this->_addressU);
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_ADDRESSV, this->_addressV);
        this->_renderer.GetDevice()->SetSamplerState(unit, D3DSAMP_ADDRESSW, this->_addressW);
    }

}}}

#endif