#ifndef __TOOLS_RENDERERS_UTILS_IMAGE_HPP__
#define __TOOLS_RENDERERS_UTILS_IMAGE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Image
    {
    private:
        static Renderers::IShaderProgram* _shader;
        static Renderers::IShaderParameter* _textureParameter;
        static Renderers::IVertexBuffer* _vertexBuffer;

        IRenderer& _renderer;
        ITexture2D* _texture;

    public:
        Image(IRenderer& renderer, std::unique_ptr<Renderers::ITexture2D> texture);
        Image(IRenderer& renderer, std::string const& imagePath);
        ~Image();

        void Render();

    private:
        static void _InitShader(IRenderer& renderer);
    };

}}}

#endif
