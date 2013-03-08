#ifndef __TOOLS_RENDERERS_UTILS_IMAGE_HPP__
#define __TOOLS_RENDERERS_UTILS_IMAGE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Image
    {
    private:
        std::unique_ptr<IVertexBuffer> _vertexBuffer;
        std::unique_ptr<IIndexBuffer> _indexBuffer;

        IRenderer& _renderer;

    public:
        Image(IRenderer& renderer);
        ~Image();

        void Render(IShaderParameter& textureParameter, ITexture2D& texture);
        void Render();

        void SetTextureCoords(glm::vec2 topLeft, glm::vec2 bottomRight);
    };

}}}

#endif
