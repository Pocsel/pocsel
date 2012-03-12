#ifdef _WIN32

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/IndexBuffer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    IndexBuffer::IndexBuffer(DX9Renderer& renderer) :
        _renderer(renderer)
    {
    }

    IndexBuffer::~IndexBuffer()
    {
    }

    void IndexBuffer::SetData(std::size_t size, void const* data)
    {
    }

    void IndexBuffer::SetSubData(std::size_t offset, std::size_t size, void const* data)
    {
    }

    void IndexBuffer::Bind()
    {
    }

    void IndexBuffer::Unbind()
    {
    }

}}}

#endif