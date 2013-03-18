#pragma once

#include "tools/Vector2.hpp"

namespace Tools { namespace Gfx {

    class GLRenderer;

    namespace OpenGL {

    class Texture2D : public ITexture2D
    {
    private:
        GLRenderer& _renderer;
        GLuint _id;
        int _bindId;
        glm::uvec2 _size;
        bool _hasAlpha;

    public:
        Texture2D(GLRenderer& renderer, PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize = glm::uvec2(0), void const* mipmapData = 0);
        Texture2D(GLRenderer& renderer, std::string const& imagePath);
        Texture2D(GLRenderer& renderer, GLuint id);
        virtual ~Texture2D();

        virtual void Bind();
        virtual void Unbind();

        GLuint GetID() const { return this->_id; }
        int GetBindID() const { return this->_bindId; }

        virtual glm::uvec2 const& GetSize() const { return this->_size; }
        virtual bool HasAlpha() const { return this->_hasAlpha; }
        virtual void SetFilters(TextureFilter::Type minFilter, TextureFilter::Type magFilter);

    private:
        void _FinishLoading(unsigned int ilID);
        void _FinishLoading(GLint internalFormat, GLenum format, Uint32 size, GLvoid const* data, int pixelSize, void const* mipmapData, bool flip);
    };

}}}
