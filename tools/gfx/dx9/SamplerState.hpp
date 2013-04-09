#pragma once

#include "tools/gfx/DX9Renderer.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    class SamplerState : public ISamplerState
    {
    private:
        DX9Renderer& _renderer;
        TextureFilter::Type _minFilter;
        TextureFilter::Type _magFilter;
        TextureFilter::Type _mipFilter;
        int _maxAnisotropy;
        int _maxLOD;
        TextureAddress::Type _addressU;
        TextureAddress::Type _addressV;
        TextureAddress::Type _addressW;

    public:
        SamplerState(DX9Renderer& renderer);
        virtual ~SamplerState();

        virtual TextureFilter::Type GetMinFilter() const { return this->_minFilter; }
        virtual TextureFilter::Type GetMagFilter() const { return this->_magFilter; }
        virtual TextureFilter::Type GetMipFilter() const { return this->_mipFilter; }
        virtual void SetMinFilter(TextureFilter::Type filter);
        virtual void SetMagFilter(TextureFilter::Type filter);
        virtual void SetMipFilter(TextureFilter::Type filter);
        virtual void SetMaxAnisotropy(int value);
        virtual void SetMaxLOD(int value);
        virtual void SetAddressU(TextureAddress::Type mode);
        virtual void SetAddressV(TextureAddress::Type mode);
        virtual void SetAddressW(TextureAddress::Type mode);

        virtual void Bind(Uint32 unit);
    };

}}}
