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
        TextureFilter::Type _mipFilter;

    public:
        SamplerState(GLRenderer& renderer);
        virtual ~SamplerState();

        virtual TextureFilter::Type GetMinFilter() const { return this->_minFilter; }
        virtual TextureFilter::Type GetMagFilter() const { return this->_magFilter; }
        virtual TextureFilter::Type GetMipFilter() const { return this->_mipFilter; }

        virtual void SetMinFilter(TextureFilter::Type filter);
        virtual void SetMagFilter(TextureFilter::Type filter);
        virtual void SetMipFilter(TextureFilter::Type filter);
        virtual void SetMaxAnisotropy(int value);
        virtual void SetMaxLOD(int value);
        virtual void SetAddressU(TextureAddress::Type mode); // S
        virtual void SetAddressV(TextureAddress::Type mode); // T
        virtual void SetAddressW(TextureAddress::Type mode); // R

        virtual void Bind(Uint32 unit);
    };

}}}
