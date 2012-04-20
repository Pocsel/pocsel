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
        this->_textures.clear();
        GLCHECK(glDeleteFramebuffersEXT(1, &this->_frameBuffer));
    }

    int RenderTarget::PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage)
    {
        this->_targets.push_back(std::make_pair(format, usage));
        return _PushRenderTarget(format, usage);
    }

    void RenderTarget::Bind()
    {
        static const GLenum bufs[] = {
            GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
            GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT,
            GL_COLOR_ATTACHMENT8_EXT, GL_COLOR_ATTACHMENT9_EXT, GL_COLOR_ATTACHMENT10_EXT, GL_COLOR_ATTACHMENT11_EXT,
            GL_COLOR_ATTACHMENT12_EXT, GL_COLOR_ATTACHMENT13_EXT, GL_COLOR_ATTACHMENT14_EXT, GL_COLOR_ATTACHMENT15_EXT,
        };

        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_frameBuffer));
        GLCHECK(glDrawBuffers(this->_colorCount, bufs));
    }

    void RenderTarget::Resize(glm::uvec2 const& newSize)
    {
        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0));
        this->_size = newSize;
        this->_textures.clear();
        GLCHECK(glDeleteFramebuffersEXT(1, &this->_frameBuffer));
        GLCHECK(glGenFramebuffersEXT(1, &this->_frameBuffer));
        this->_colorCount = 0;
        for (auto it = this->_targets.begin(), ite = this->_targets.end(); it != ite; ++it)
            this->_PushRenderTarget(it->first, it->second);
    }

    int RenderTarget::_PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage)
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
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));

        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER, this->_frameBuffer));
        if (usage == RenderTargetUsage::Color)
        {
            GLCHECK(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + this->_colorCount, GL_TEXTURE_2D, textureId, 0));
            this->_colorCount++;
        }
        if (usage == RenderTargetUsage::Depth || usage == RenderTargetUsage::DepthStencil)
            GLCHECK(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, textureId, 0));
        if (usage == RenderTargetUsage::Stencil || usage == RenderTargetUsage::DepthStencil)
            GLCHECK(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, textureId, 0));
        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER, 0));

        int idx = (int)this->_textures.size();
        this->_textures.push_back(std::unique_ptr<ITexture2D>(new Texture2D(this->_renderer, textureId)));
        return idx;
    }

}}}
