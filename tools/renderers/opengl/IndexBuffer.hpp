#ifndef __TOOLS_RENDERERS_OPENGL_INDEXBUFFER_HPP__
#define __TOOLS_RENDERERS_OPENGL_INDEXBUFFER_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class IndexBuffer : public IIndexBuffer
    {
    private:
        GLuint _id;

    public:
        IndexBuffer();
        virtual ~IndexBuffer();

        virtual void SetData(std::size_t size, void const* data);
        virtual void SetSubData(std::size_t offset, std::size_t size, void const* data);

        virtual void Bind();
        virtual void Unbind();
    };

}}}

#endif
