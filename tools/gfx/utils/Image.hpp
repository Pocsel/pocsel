#pragma once

#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace Utils {

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
