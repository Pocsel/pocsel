#ifndef __TOOLS_PRIMITIVES_IMAGE_HPP__
#define __TOOLS_PRIMITIVES_IMAGE_HPP__

#include <memory>

#include "tools/IRenderer.hpp"
#include "IPrimitive.hpp"

namespace Tools { namespace Gui { namespace Primitives {

    class Image : public IPrimitive
    {
    private:
        static Renderers::IVertexBuffer* _vertexBuffer;
        static Renderers::IShaderProgram* _shader;
        static Renderers::IShaderParameter* _textureParameter;
        static void (*_renderFunc)(Image&, IRenderer&);

        Renderers::ITexture2D* _texture;
        Renderers::IVertexBuffer* _uvBuffer;

    public:
        Image(std::unique_ptr<Renderers::ITexture2D> texture);
        Image(IRenderer& r, std::string const& imagePath);
        virtual ~Image();

        virtual void Render(IRenderer& renderer) { Image::_renderFunc(*this, renderer); }

    private:
        void _CreateFromTexture(std::unique_ptr<Renderers::ITexture2D> texture);

        static void _InitRender(Image& img, IRenderer& renderer);
        static void _Render(Image& img, IRenderer& renderer);
    };

}}}

#endif
