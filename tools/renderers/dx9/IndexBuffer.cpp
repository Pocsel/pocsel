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
        this->_renderer.GetDevice()->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, GetIndexBufferFormat(indicesType), D3DPOOL_DEFAULT, &this->_indexBuffer, 0);
        this->SetSubData(0, size, data);
    }

    void IndexBuffer::SetSubData(std::size_t offset, std::size_t size, void const* data)
    {
        void* ptr;
        this->_indexBuffer->Lock(offset, size, &ptr, 0);
        std::memcpy(ptr, data, size);
        this->_indexBuffer->Unlock();
    }

    void IndexBuffer::Bind()
    {
        this->_renderer.GetDevice()->SetIndices(this->_indexBuffer);
    }

    void IndexBuffer::Unbind()
    {
        this->_renderer.GetDevice()->SetIndices(0);
    }

}}}

#endif