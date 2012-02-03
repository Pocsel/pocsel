#include <cassert>

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/IndexBuffer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    IndexBuffer::IndexBuffer()
    {
        GLCHECK(glGenBuffersARB(1, &this->_id));
    }

    IndexBuffer::~IndexBuffer()
    {
        GLCHECK(glDeleteBuffersARB(1, &this->_id));
    }

    void IndexBuffer::SetData(std::size_t size, void const* data)
    {
        this->Bind();
        GLCHECK(glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size, data, GL_STATIC_DRAW_ARB));
        this->Unbind();
    }

    void IndexBuffer::SetSubData(std::size_t offset, std::size_t size, void const* data)
    {
        this->Bind();
        GLCHECK(glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, offset, size, data));
        this->Unbind();
    }

    void IndexBuffer::Bind()
    {
        GLCHECK(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, this->_id));
    }

    void IndexBuffer::Unbind()
    {
        GLCHECK(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    }

}}}
