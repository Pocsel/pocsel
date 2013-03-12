#pragma once

#include "tools/Vector2.hpp"

namespace Tools { namespace Gfx {

    class DX9Renderer;

    namespace DX9 {

    class Texture2D : public ITexture2D
    {
    private:
        DX9Renderer& _renderer;
        glm::uvec2 _size;
        bool _hasAlpha;
        ComPtr<IDirect3DTexture9> _texture;
        int _bindId;

    public:
        Texture2D(DX9Renderer& renderer, PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize = glm::uvec2(0), void const* mipmapData = 0);
        Texture2D(DX9Renderer& renderer, std::string const& imagePath);
        Texture2D(DX9Renderer& renderer, IDirect3DTexture9* texture);
        virtual ~Texture2D();

        virtual void Bind();
        virtual void Unbind();

        IDirect3DTexture9* GetTexture() const { return this->_texture.get(); }
        virtual glm::uvec2 const& GetSize() const { return this->_size; }
        virtual bool HasAlpha() const { return this->_hasAlpha; }
        virtual void SetFilters(TextureFilter::Type minFilter, TextureFilter::Type magFilter);

    private:
        void _FinishLoading(glm::u8vec4 const* data, std::size_t size, void const* mipmapData);
        void _FinishLoading(unsigned int ilID);
    };

}}}
