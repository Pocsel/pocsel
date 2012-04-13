#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/VertexBuffer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    VertexBuffer::VertexBuffer()
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
        this->_stride += this->_attributes[this->_nbAttrib].nbElements * static_cast<GLuint>(DataType::GetSize(type));
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

    static int nbTexCoordActive = 0;
    void VertexBuffer::Bind()
    {
        GLCHECK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, this->_id));

        auto it = this->_attributes,
            itEnd = this->_attributes + this->_nbAttrib;
        for (; it != itEnd; ++it)
        {
            switch (it->location)
            {
            case VertexAttributeUsage::Position:
                GLCHECK(glVertexPointer(it->nbElements, it->type, this->_stride, it->offset));
                GLCHECK(glEnableClientState(GL_VERTEX_ARRAY));
                break;

            case VertexAttributeUsage::Normal:
                GLCHECK(glNormalPointer(it->type, this->_stride, it->offset));
                GLCHECK(glEnableClientState(GL_NORMAL_ARRAY));
                break;

            case VertexAttributeUsage::Color:
                GLCHECK(glColorPointer(it->nbElements, it->type, this->_stride, it->offset));
                GLCHECK(glEnableClientState(GL_COLOR_ARRAY));
                break;

            case VertexAttributeUsage::TexCoord:
                GLCHECK(glClientActiveTextureARB(GL_TEXTURE0_ARB));
                GLCHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                GLCHECK(glTexCoordPointer(it->nbElements, it->type, this->_stride, it->offset));
                break;

            case VertexAttributeUsage::Custom1:
                GLCHECK(glClientActiveTextureARB(GL_TEXTURE1_ARB));
                GLCHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                GLCHECK(glTexCoordPointer(it->nbElements, it->type, this->_stride, it->offset));
                break;

            case VertexAttributeUsage::Custom2:
                GLCHECK(glClientActiveTextureARB(GL_TEXTURE2_ARB));
                GLCHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                GLCHECK(glTexCoordPointer(it->nbElements, it->type, this->_stride, it->offset));
                break;

            default:
                GLCHECK(glVertexAttribPointerARB(it->location, it->nbElements, it->type, GL_FALSE, this->_stride, it->offset));
                GLCHECK(glEnableVertexAttribArrayARB(it->location));
                break;
            }
        }
    }

    void VertexBuffer::Unbind()
    {
        GLCHECK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
        auto it = this->_attributes,
            itEnd = this->_attributes + this->_nbAttrib;
        for (; it != itEnd; ++it)
        {
            switch (it->location)
            {
            case VertexAttributeUsage::Position:
                GLCHECK(glDisableClientState(GL_VERTEX_ARRAY));
                break;

            case VertexAttributeUsage::Normal:
                GLCHECK(glDisableClientState(GL_NORMAL_ARRAY));
                break;

            case VertexAttributeUsage::Color:
                GLCHECK(glDisableClientState(GL_COLOR_ARRAY));
                break;

            case VertexAttributeUsage::TexCoord:
                GLCHECK(glClientActiveTextureARB(GL_TEXTURE0_ARB));
                GLCHECK(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
                break;

            case VertexAttributeUsage::Custom1:
                GLCHECK(glClientActiveTextureARB(GL_TEXTURE1_ARB));
                GLCHECK(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
                break;

            case VertexAttributeUsage::Custom2:
                GLCHECK(glClientActiveTextureARB(GL_TEXTURE2_ARB));
                GLCHECK(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
                break;

            default:
                GLCHECK(glDisableVertexAttribArrayARB(it->location));
                break;
            }
        }
        GLCHECK(glClientActiveTextureARB(GL_TEXTURE0_ARB));
    }

}}}
