#ifndef __TOOLS_RENDERERS_OPENGL_INDEXBUFFER_HPP__
#define __TOOLS_RENDERERS_OPENGL_INDEXBUFFER_HPP__

#include "tools/renderers/GLRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

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

#endif
