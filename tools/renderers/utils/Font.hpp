#ifndef __TOOLS_RENDERERS_UTILS_FONT_HPP__
#define __TOOLS_RENDERERS_UTILS_FONT_HPP__

#include "tools/Color.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Font
    {
    private:
        struct FontData;
        struct CharInfo
        {
            Vector2f advance;
            Vector2f offset;
            Vector2f size;
            Vector2f texUVmin;
            Vector2f texUVmax;
        };

        static const int startChar = ' ';
        static const int endChar = 0xff;//'~';

    private:
        static Renderers::IShaderProgram* _shader;
        static Renderers::IShaderParameter* _shaderTexture;
        static Renderers::IShaderParameter* _shaderColor;
        static Renderers::IShaderParameter* _shaderMultZ;

        IRenderer& _renderer;
        FontData* _data;
        ITexture2D* _texture;
        IVertexBuffer* _vertexBuffer;
        std::vector<CharInfo> _charInfos;

    public:
        Font(IRenderer& renderer, std::string const& file, std::size_t fontSize);
        Font(IRenderer& renderer, void const* data, std::size_t dataLength, std::size_t fontSize);
        ~Font();

        void Render(std::string const& text, Color4f const& color = Color4f(0, 0, 0, 1), bool invert = false);

#ifdef  DEBUG
        void RenderTextureDebug();
#endif
    private:
        void _InitTextures();
        static void _InitShaders(IRenderer& renderer);
    };

}}}

#endif
