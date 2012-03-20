#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/IndexBuffer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    IndexBuffer::IndexBuffer(DX9Renderer& renderer) :
        _renderer(renderer),
        _indexBuffer(0)
    {
    }

    IndexBuffer::~IndexBuffer()
    {
        if (this->_indexBuffer)
            this->_indexBuffer->Release();
    }

    void IndexBuffer::SetData(DataType::Type indicesType, std::size_t size, void const* data)
    {
        if (this->_indexBuffer)
            this->_indexBuffer->Release();
        DXCHECKERROR(this->_renderer.GetDevice()->CreateIndexBuffer((UINT)size, D3DUSAGE_WRITEONLY, GetIndexBufferFormat(indicesType), D3DPOOL_MANAGED, &this->_indexBuffer, 0));
        this->SetSubData(0, size, data);
    }

    void IndexBuffer::SetSubData(std::size_t offset, std::size_t size, void const* data)
    {
        void* ptr;
        DXCHECKERROR(this->_indexBuffer->Lock((UINT)offset, (UINT)size, &ptr, 0));
        std::memcpy(ptr, data, size);
        DXCHECKERROR(this->_indexBuffer->Unlock());
    }

    void IndexBuffer::Bind()
    {
        DXCHECKERROR(this->_renderer.GetDevice()->SetIndices(this->_indexBuffer));
    }

    void IndexBuffer::Unbind()
    {
        DXCHECKERROR(this->_renderer.GetDevice()->SetIndices(0));
    }

}}}

#endif