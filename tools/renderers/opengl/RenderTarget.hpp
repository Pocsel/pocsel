#ifndef __TOOLS_RENDERERS_OPENGL_RENDERTARGET_HPP__
#define __TOOLS_RENDERERS_OPENGL_RENDERTARGET_HPP__

#include "tools/renderers/GLRenderer.hpp"
#include "tools/renderers/opengl/Texture2D.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class RenderTarget : public IRenderTarget
    {
    private:
        GLRenderer& _renderer;
        GLuint _frameBuffer;
        GLuint _renderBuffer;
        Texture2D* _texture;

    public:
        RenderTarget(GLRenderer& renderer, Vector2u const& size);
        virtual ~RenderTarget();

        virtual void Bind();

        virtual ITexture2D& GetTexture() { return *this->_texture; }
    };

}}}

#endif
