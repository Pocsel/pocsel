#pragma once

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/utils/texture/ITexture.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Texture {

    class Texture : public ITexture
    {
    private:
        ITexture2D& _texture;

    public:
        Texture(ITexture2D& texture) : _texture(texture) {}
        virtual ~Texture() {}

        virtual ITexture* Clone() { return new Texture(*this); }

        virtual void Update(Uint64) {} // totalTime in microsecond
        virtual void Bind() { this->_texture.Bind(); }
        virtual void Unbind() { this->_texture.Unbind(); }

        virtual ITexture2D& GetCurrentTexture() { return this->_texture; }
        virtual ITexture2D const& GetCurrentTexture() const { return this->_texture; }
        virtual bool HasAlpha() const { return this->_texture.HasAlpha(); }
    };

}}}}
