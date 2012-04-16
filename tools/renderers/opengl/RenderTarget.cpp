#include "tools/precompiled.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/RenderTarget.hpp"
#include "tools/renderers/opengl/Texture2D.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    RenderTarget::RenderTarget(GLRenderer& renderer, glm::uvec2 const& size) :
        _renderer(renderer)
    {
        GLCHECK(glGenFramebuffersEXT(1, &this->_frameBuffer));
        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_frameBuffer));

        GLCHECK(glGenRenderbuffersEXT(1, &this->_renderBuffer));
        GLCHECK(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_renderBuffer));
        GLCHECK(glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, size.x, size.y));
        GLCHECK(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0));

        GLuint textureId;
        GLCHECK(glGenTextures(1, &textureId));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, textureId));
        GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
        this->_texture = new Texture2D(renderer, textureId);

        // attach a texture to FBO color attachement point
        GLCHECK(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureId, 0));

        // attach a renderbuffer to depth attachment point
        GLCHECK(glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_renderBuffer));
    }

    RenderTarget::~RenderTarget()
    {
        GLCHECK(glDeleteRenderbuffersEXT(1, &this->_renderBuffer));
        GLCHECK(glDeleteFramebuffersEXT(1, &this->_frameBuffer));
        Delete(this->_texture);
    }

    void RenderTarget::Bind()
    {
        GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_frameBuffer));
    }

}}}
