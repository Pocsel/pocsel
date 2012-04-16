#include "tools/precompiled.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/RenderTarget.hpp"
#include "tools/renderers/opengl/Texture2D.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    RenderTarget::RenderTarget(GLRenderer& renderer, glm::uvec2 const& size) :
        _renderer(renderer),
        _size(size),
        _colorCount(0)
    {
        GLCHECK(glGenFramebuffersEXT(1, &this->_frameBuffer));
    }

    RenderTarget::~RenderTarget()
    {
        GLCHECK(glDeleteFramebuffersEXT(1, &this->_frameBuffer));
    }

    int RenderTarget::PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage)
    {
        GLuint textureId;
        GLCHECK(glGenTextures(1, &textureId));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, textureId));
        GLCHECK(glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GetInternalFormatFromPixelFormat(format),
            this->_size.x, this->_size.y, 0,
            GetFormatFromPixelFormat(format),
            GetTypeFromPixelFormat(format),
            0));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));

        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER, this->_frameBuffer));
        if (usage == RenderTargetUsage::Color)
            GLCHECK(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0 + this->_colorCount++, GL_TEXTURE_2D, textureId, 0));
        if (usage == RenderTargetUsage::Depth || usage == RenderTargetUsage::DepthStencil)
            GLCHECK(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0));
        if (usage == RenderTargetUsage::Stencil || usage == RenderTargetUsage::DepthStencil)
            GLCHECK(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureId, 0));
        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER, 0));

        int idx = (int)this->_textures.size();
        this->_textures.push_back(std::unique_ptr<ITexture2D>(new Texture2D(this->_renderer, textureId)));
        return idx;
    }

    void RenderTarget::Bind()
    {
        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_frameBuffer));
    }

}}}
