#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/Program.hpp"
#include "tools/renderers/opengl/VertexBuffer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    VertexBuffer::VertexBuffer(GLRenderer& renderer) :
        _renderer(renderer)
    {
        this->_stride = 0;
        this->_nbAttrib = 0;
        GLCHECK(glGenBuffersARB(1, &this->_id));
    }

    VertexBuffer::~VertexBuffer()
    {
        GLCHECK(glDeleteBuffersARB(1, &this->_id));
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
        this->Bind();
        GLCHECK(glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, data, GetVertexBufferUsage(usage)));
        this->Unbind();
    }

    void VertexBuffer::SetSubData(std::size_t offset, std::size_t size, void const* data)
    {
        GLCHECK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, this->_id));
        GLCHECK(glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, size, data));
        GLCHECK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    }

    void VertexBuffer::Bind()
    {
        GLCHECK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, this->_id));
        this->_renderer.bindedVertexBuffer = this;

        auto program = dynamic_cast<Program*>(&this->_renderer.GetCurrentProgram());
        if (program == nullptr)
        {
            for (auto it = this->_attributes, itEnd = this->_attributes + this->_nbAttrib; it != itEnd; ++it)
            {
                GLCHECK(glVertexAttribPointerARB(GetVertexAttributeIndex(it->location), it->nbElements, it->type, GL_FALSE, this->_stride, it->offset));
                GLCHECK(glEnableVertexAttribArrayARB(GetVertexAttributeIndex(it->location)));
            }
        }
        else
        {
            for (auto it = this->_attributes, itEnd = this->_attributes + this->_nbAttrib; it != itEnd; ++it)
            {
                GLCHECK(glVertexAttribPointer(program->GetAttributeIndex(it->location), it->nbElements, it->type, GL_FALSE, this->_stride, it->offset));
                GLCHECK(glEnableVertexAttribArray(program->GetAttributeIndex(it->location)));
            }
        }
    }

    void VertexBuffer::Unbind()
    {
        this->_renderer.bindedVertexBuffer = nullptr;
        GLCHECK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));

        auto program = dynamic_cast<Program*>(&this->_renderer.GetCurrentProgram());
        if (program == nullptr)
        {
            for (auto it = this->_attributes, itEnd = this->_attributes + this->_nbAttrib; it != itEnd; ++it)
                GLCHECK(glDisableVertexAttribArrayARB(GetVertexAttributeIndex(it->location)));
        }
        else
        {
            for (auto it = this->_attributes, itEnd = this->_attributes + this->_nbAttrib; it != itEnd; ++it)
                GLCHECK(glDisableVertexAttribArray(program->GetAttributeIndex(it->location)));
        }
    }

}}}
