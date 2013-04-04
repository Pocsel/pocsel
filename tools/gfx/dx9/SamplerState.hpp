#pragma once

#include "tools/gfx/DX9Renderer.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    class SamplerState : public ISamplerState
    {
    private:
        DX9Renderer& _renderer;
        TextureFilter::Type _minFilter;
        TextureFilter::Type _magFilter;

    public:
        SamplerState(DX9Renderer& renderer);
        virtual ~SamplerState();
        
        virtual TextureFilter::Type GetMinFilter() const { return this->_minFilter; }
        virtual TextureFilter::Type GetMagFilter() const { return this->_magFilter; }
        virtual void SetMinFilter(TextureFilter::Type filter);
        virtual void SetMagFilter(TextureFilter::Type filter);

        virtual void Bind(Uint32 unit);
    };

}}}
