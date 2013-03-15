#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/opengl/Program.hpp"
#include "tools/gfx/opengl/VertexBuffer.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    VertexBuffer::VertexBuffer(GLRenderer& renderer) :
        _renderer(renderer)
    {
        this->_stride = 0;
        this->_nbAttrib = 0;
        GLCHECK(glGenBuffers(1, &this->_id));
    }

    VertexBuffer::~VertexBuffer()
    {
        GLCHECK(glDeleteBuffers(1, &this->_id));
    }

    void VertexBuffer::PushVertexAttribute(DataType::Type type, VertexAttributeUsage::Type usage, Uint32 nbElements)
    {
        assert(this->_nbAttrib < MaxAttributes && "Too many vertex attributes");
        this->_attributes[this->_nbAttrib].location = usage;
        this->_attributes[this->_nbAttrib].offset = reinterpret_cast<GLvoid*>(this->_stride);
        this->_attributes[this->_nbAttrib].nbElements = nbElements;
        this->_attributes[this->_nbAttrib].type = GetTypeFromDataType(type);
        this->_attributes[this->_nbAttrib].size = this->_attributes[this->_nbAttrib].nbElements * static_cast<GLuint>(DataType::GetSize(type));
        this->_stride += this->_attributes[this->_nbAttrib].size;
        this->_nbAttrib++;
    }

    void VertexBuffer::SetData(std::size_t size, void const* data, VertexBufferUsage::Type usage)
    {
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, this->_id));
        GLCHECK(glBufferData(GL_ARRAY_BUFFER, size, data, GetVertexBufferUsage(usage)));
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void VertexBuffer::SetSubData(std::size_t offset, std::size_t size, void const* data)
    {
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, this->_id));
        GLCHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void VertexBuffer::Bind()
    {
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, this->_id));
        this->_renderer.bindedVertexBuffer = this;

        auto program = (Program*)this->_renderer.GetCurrentProgram();
        assert(program != nullptr && "il faut un shader pour bind");
        for (auto it = this->_attributes, itEnd = this->_attributes + this->_nbAttrib; it != itEnd; ++it)
        {
            auto idx = program->GetAttributeIndex(it->location);
            if (idx >= 0)
            {
                GLCHECK(glVertexAttribPointer(idx, it->nbElements, it->type, GL_FALSE, this->_stride, it->offset));
                GLCHECK(glEnableVertexAttribArray(idx));
            }
        }
    }

    void VertexBuffer::Unbind()
    {
        auto program = (Program*)this->_renderer.GetCurrentProgram();
        assert(program != nullptr && "il faut un shader pour bind");
        for (auto it = this->_attributes, itEnd = this->_attributes + this->_nbAttrib; it != itEnd; ++it)
        {
            auto idx = program->GetAttributeIndex(it->location);
            if (idx >= 0)
                GLCHECK(glDisableVertexAttribArray(idx));
        }

        this->_renderer.bindedVertexBuffer = nullptr;
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

}}}
