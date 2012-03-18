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
        IRenderer& _renderer;
        FontData* _data;
        std::unique_ptr<ITexture2D> _texture;
        std::unique_ptr<IVertexBuffer> _vertexBuffer;
        std::vector<CharInfo> _charInfos;

    public:
        Font(IRenderer& renderer, std::string const& file, std::size_t fontSize);
        Font(IRenderer& renderer, void const* data, std::size_t dataLength, std::size_t fontSize);
        ~Font();

        void Render(IShaderParameter& textureParameter, std::string const& text, bool invert = false);

    private:
        void _InitTextures();
    };

}}}

#endif
