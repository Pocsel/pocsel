#ifndef __TOOLS_RENDERERS_UTILS_IMAGE_HPP__
#define __TOOLS_RENDERERS_UTILS_IMAGE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Image
    {
    private:
        static IVertexBuffer* _vertexBuffer;
        static IIndexBuffer* _indexBuffer;

        IRenderer& _renderer;

    public:
        Image(IRenderer& renderer);
        ~Image();

        void Render(IShaderParameter& textureParameter, ITexture2D& texture);

    private:
        static void _InitBuffers(IRenderer& renderer);
        static void _DeleteBuffers();
    };

}}}

#endif
