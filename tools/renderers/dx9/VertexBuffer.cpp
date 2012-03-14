#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/VertexBuffer.hpp"
#include "tools/renderers/DX9Renderer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    VertexBuffer::VertexBuffer(DX9Renderer& renderer) :
        _renderer(renderer),
        _vertexBuffer(0),
        _size(0),
        _nbAttrib(0),
        _stride(0),
        _vertexDeclaration(0)
    {
    }

    VertexBuffer::~VertexBuffer()
    {
        if (this->_vertexDeclaration)
            this->_vertexDeclaration->Release();
        if (this->_vertexBuffer)
            this->_vertexBuffer->Release();
    }

    void VertexBuffer::PushVertexAttribute(DataType::Type type, VertexAttributeUsage::Type usage, Uint32 nbElements)
    {
        static D3DVERTEXELEMENT9 end = D3DDECL_END();
        assert(this->_nbAttrib < MaxAttributes && "Too many vertex attributes");
        //this->_attributes[this->_nbAttrib].usage = usage;
        //this->_attributes[this->_nbAttrib].offset = this->_stride;
        //this->_attributes[this->_nbAttrib].nbElements = nbElements;
        //this->_attributes[this->_nbAttrib].type = GetTypeFromDataTypeAndNbElements(type, nbElements);
        this->_attributes[this->_nbAttrib].Stream = 0;
        this->_attributes[this->_nbAttrib].Offset = this->_stride;
        this->_attributes[this->_nbAttrib].Type = GetTypeFromDataTypeAndNbElements(type, nbElements);
        this->_attributes[this->_nbAttrib].Method = D3DDECLMETHOD_DEFAULT;
        this->_attributes[this->_nbAttrib].Usage = GetVertexAttributeUsage(usage);
        this->_attributes[this->_nbAttrib].UsageIndex = GetVertexAttributeUsageIndex(usage);
        this->_stride += nbElements * DataType::GetSize(type);
        this->_nbAttrib++;
        std::memcpy(&(this->_attributes[this->_nbAttrib]), &end, sizeof(end));
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
        if (this->_vertexDeclaration == 0)
            this->_renderer.GetDevice()->CreateVertexDeclaration(this->_attributes, &this->_vertexDeclaration);
        this->_renderer.GetDevice()->SetVertexDeclaration(this->_vertexDeclaration);
        this->_renderer.GetDevice()->SetStreamSource(0, this->_vertexBuffer, 0, this->_stride);
    }

    void VertexBuffer::Unbind()
    {
        this->_renderer.GetDevice()->SetStreamSource(0, 0, 0, 0);
    }

}}}

#endif