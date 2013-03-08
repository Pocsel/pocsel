#pragma once

#include "tools/gfx/GLRenderer.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    class IndexBuffer : public IIndexBuffer
    {
    private:
        GLRenderer& _renderer;
        GLuint _id;
        DataType::Type _type;

    public:
        IndexBuffer(GLRenderer& renderer);
        virtual ~IndexBuffer();

        virtual void SetData(DataType::Type indicesType, std::size_t size, void const* data);
        virtual void SetSubData(std::size_t offset, std::size_t size, void const* data);

        virtual void Bind();
        virtual void Unbind();

        DataType::Type GetType() const { return this->_type; }
    };

}}}
