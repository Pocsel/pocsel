#pragma once

#include "tools/gfx/GLRenderer.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    class SamplerState : public ISamplerState
    {
    private:
        GLRenderer& _renderer;
        GLuint _id;
        TextureFilter::Type _minFilter;
        TextureFilter::Type _magFilter;

    public:
        SamplerState(GLRenderer& renderer);
        virtual ~SamplerState();
        
        virtual TextureFilter::Type GetMinFilter() const { return this->_minFilter; }
        virtual TextureFilter::Type GetMagFilter() const { return this->_magFilter; }
        virtual void SetMinFilter(TextureFilter::Type filter);
        virtual void SetMagFilter(TextureFilter::Type filter);

        virtual void Bind(Uint32 unit);
    };

}}}
