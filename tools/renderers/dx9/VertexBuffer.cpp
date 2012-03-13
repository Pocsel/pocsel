#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/VertexBuffer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    VertexBuffer::VertexBuffer(DX9Renderer& renderer) :
        _renderer(renderer),
        _vertexBuffer(0),
        _size(0)
    {
    }

    VertexBuffer::~VertexBuffer()
    {
        if (this->_vertexBuffer)
            this->_vertexBuffer->Release();
    }

    void VertexBuffer::PushVertexAttribute(DataType::Type type, VertexAttributeUsage::Type usage, Uint32 nbElements)
    {
        //assert(this->_nbAttrib < MaxAttributes && "Too many vertex attributes");
        //this->_attributes[this->_nbAttrib].location = usage;
        //this->_attributes[this->_nbAttrib].offset = reinterpret_cast<GLvoid*>(this->_stride);
        //this->_attributes[this->_nbAttrib].nbElements = nbElements;
        //this->_attributes[this->_nbAttrib].type = GetTypeFromDataType(type);
        //this->_stride += this->_attributes[this->_nbAttrib].nbElements * static_cast<GLuint>(DataType::GetSize(type));
        //this->_nbAttrib++;
    }

    void VertexBuffer::SetData(std::size_t size, void const* data, VertexBufferUsage::Type usage)
    {
        if (this->_size < size || this->_size * 80 / 100 > size)
        {
            if (this->_vertexBuffer)
                this->_vertexBuffer->Release();
            HRESULT ret = this->_renderer.GetDevice()->CreateVertexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &this->_vertexBuffer, 0);
            if (ret < 0)
                throw std::runtime_error("DirectX: Could not create a vertex buffer");
            this->_size = size;
        }
        if (data == 0)
            return;
        this->SetSubData(0, size, data);
    }

    void VertexBuffer::SetSubData(std::size_t offset, std::size_t size, void const* data)
    {
        void* ptr;
        HRESULT ret = this->_vertexBuffer->Lock(offset, size, &ptr, 0); // TODO Check
        if (ret < 0)
            throw std::runtime_error("DirectX: Could not create a vertex buffer");
        std::memcpy(ptr, data, size);
        this->_vertexBuffer->Unlock();
    }

    static int nbTexCoordActive = 0;
    void VertexBuffer::Bind()
    {
        //GLCHECK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, this->_id));

        //auto it = this->_attributes,
        //    itEnd = this->_attributes + this->_nbAttrib;
        //for (; it != itEnd; ++it)
        //{
        //    switch (it->location)
        //    {
        //    case VertexAttributeUsage::Position:
        //        GLCHECK(glVertexPointer(it->nbElements, it->type, this->_stride, it->offset));
        //        GLCHECK(glEnableClientState(GL_VERTEX_ARRAY));
        //        break;

        //    case VertexAttributeUsage::Normal:
        //        GLCHECK(glNormalPointer(it->type, this->_stride, it->offset));
        //        GLCHECK(glEnableClientState(GL_NORMAL_ARRAY));
        //        break;

        //    case VertexAttributeUsage::Color:
        //        GLCHECK(glColorPointer(it->nbElements, it->type, this->_stride, it->offset));
        //        GLCHECK(glEnableClientState(GL_COLOR_ARRAY));
        //        break;

        //    case VertexAttributeUsage::TexCoord:
        //        GLCHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
        //        GLCHECK(glTexCoordPointer(it->nbElements, it->type, this->_stride, it->offset));
        //        break;

        //    default:
        //        GLCHECK(glVertexAttribPointerARB(it->location, it->nbElements, it->type, GL_FALSE, this->_stride, it->offset));
        //        GLCHECK(glEnableVertexAttribArrayARB(it->location));
        //        break;
        //    }
        //}
    }

    void VertexBuffer::Unbind()
    {
        //GLCHECK(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
        //auto it = this->_attributes,
        //    itEnd = this->_attributes + this->_nbAttrib;
        //for (; it != itEnd; ++it)
        //{
        //    switch (it->location)
        //    {
        //    case VertexAttributeUsage::Position:
        //        GLCHECK(glDisableClientState(GL_VERTEX_ARRAY));
        //        break;

        //    case VertexAttributeUsage::Normal:
        //        GLCHECK(glDisableClientState(GL_NORMAL_ARRAY));
        //        break;

        //    case VertexAttributeUsage::Color:
        //        GLCHECK(glDisableClientState(GL_COLOR_ARRAY));
        //        break;

        //    case VertexAttributeUsage::TexCoord:
        //        GLCHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
        //        break;

        //    default:
        //        GLCHECK(glDisableVertexAttribArrayARB(it->location));
        //        break;
        //    }
        //}
    }

}}}

#endif