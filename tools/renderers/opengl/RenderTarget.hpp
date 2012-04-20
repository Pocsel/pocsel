#ifndef __TOOLS_RENDERERS_OPENGL_RENDERTARGET_HPP__
#define __TOOLS_RENDERERS_OPENGL_RENDERTARGET_HPP__

#include "tools/renderers/GLRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class RenderTarget : public IRenderTarget
    {
    private:
        GLRenderer& _renderer;
        glm::uvec2 _size;
        GLuint _frameBuffer;
        std::vector<std::unique_ptr<ITexture2D>> _textures;
        int _colorCount;

        std::vector<std::pair<PixelFormat::Type, RenderTargetUsage::Type>> _targets;

    public:
        RenderTarget(GLRenderer& renderer, glm::uvec2 const& size);
        virtual ~RenderTarget();

        virtual int PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage);
        virtual void Bind();
        virtual void Resize(glm::uvec2 const& newSize);

        virtual ITexture2D& GetTexture(int idx) { return *(this->_textures[idx].get()); }
        virtual glm::uvec2 const& GetSize() const { return this->_size; }
    private:
        int _PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage);
    };

}}}

#endif
