#ifndef __TOOLS_RENDERERS_DX9_TEXTURE2D_HPP__
#define __TOOLS_RENDERERS_DX9_TEXTURE2D_HPP__

#include "tools/Color.hpp"
#include "tools/Vector2.hpp"

namespace Tools { namespace Renderers {

    class DX9Renderer;

    namespace DX9 {

    class Texture2D : public ITexture2D
    {
    private:
        DX9Renderer& _renderer;
        Vector2u _size;
        bool _hasAlpha;

    public:
        Texture2D(DX9Renderer& renderer, PixelFormat::Type format, Uint32 size, void const* data, Vector2u const& imgSize = Vector2u(0), void const* mipmapData = 0);
        Texture2D(DX9Renderer& renderer, std::string const& imagePath);
        virtual ~Texture2D();

        virtual void Bind();
        virtual void Unbind();

        virtual Vector2u const& GetSize() const { return this->_size; }
        virtual bool HasAlpha() const { return this->_hasAlpha; }
        virtual void SetFilters(TextureFilter::Type minFilter, TextureFilter::Type magFilter);

    private:
        void _FinishLoading(unsigned int ilID);
        void _FinishLoading(GLint internalFormat, GLenum format, GLvoid const* data, int pixelSize, void const* mipmapData);
    };

}}}

#endif
