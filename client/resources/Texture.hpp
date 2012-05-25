#ifndef __CLIENT_RESOURCES_TEXTURE_HPP__
#define __CLIENT_RESOURCES_TEXTURE_HPP__

#include "tools/IRenderer.hpp"
#include "client/resources/ITexture.hpp"

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Resources {

    class Texture : public ITexture
    {
    private:
        Tools::Renderers::ITexture2D* _texture;

    public:
        Texture(Tools::Renderers::ITexture2D& texture) : _texture(&texture) {}
        virtual ~Texture() {}

        virtual void Update(Uint64) {} // totalTime in microsecond
        virtual void Bind() { this->_texture->Bind(); }
        virtual void Unbind() { this->_texture->Unbind(); }

        virtual Tools::Renderers::ITexture2D& GetCurrentTexture() { return *this->_texture; }
        virtual Tools::Renderers::ITexture2D const& GetCurrentTexture() const { return *this->_texture; }
        virtual bool HasAlpha() const { return this->_texture->HasAlpha(); }
    };

}}

#endif
