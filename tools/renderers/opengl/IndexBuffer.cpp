#include "tools/precompiled.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/IndexBuffer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    IndexBuffer::IndexBuffer(GLRenderer& renderer) :
        _renderer(renderer)
    {
        GLCHECK(glGenBuffersARB(1, &this->_id));
    }

    IndexBuffer::~IndexBuffer()
    {
        GLCHECK(glDeleteBuffersARB(1, &this->_id));
    }

    void IndexBuffer::SetData(DataType::Type indicesType, std::size_t size, void const* data)
    {
        this->_type = indicesType;
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
        this->_renderer.bindedIndexBuffer = this;
        GLCHECK(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, this->_id));
    }

    void IndexBuffer::Unbind()
    {
        GLCHECK(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
        this->_renderer.bindedIndexBuffer = 0;
    }

}}}
