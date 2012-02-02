#ifndef __TOOLS_GUI_FONT_HPP__
#define __TOOLS_GUI_FONT_HPP__

#include <functional>
#include <string>
#include <vector>

#include "tools/Color.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"

namespace Tools { namespace Gui {

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
        static void (*_renderBase)(Font&, IRenderer& renderer, Color4f const&, std::string const&, bool);
        static Renderers::IShaderProgram* _shader;
        static Renderers::IShaderParameter* _shaderTexture;
        static Renderers::IShaderParameter* _shaderColor;
        static Renderers::IShaderParameter* _shaderMultZ;

        void (Font::* _render)(IRenderer& renderer, Color4f const&, std::string const&, bool);
        FontData* _data;
        Renderers::ITexture2D* _texture;
        Renderers::IVertexBuffer* _vertexBuffer;
        std::vector<CharInfo> _charInfos;

    public:
        Font(std::string const& file,
             std::size_t fontSize);
        Font(void const* data, std::size_t dataLength,
             std::size_t fontSize);
        ~Font();

        void Render(IRenderer& renderer, Color4f const& color, std::string const& text, bool invert = false)
        {
            Font::_renderBase(*this, renderer, color, text, invert);
        }

#ifdef  DEBUG
        void RenderTextureDebug(IRenderer& renderer);
#endif

    private:
        void _Render(IRenderer& renderer, Color4f const& color, std::string const& text, bool invert);
        static void _InitRenderBase(Font& font, IRenderer& renderer, Color4f const& color, std::string const& text, bool invert);
        static void _RenderBase(Font& font, IRenderer& renderer, Color4f const& color, std::string const& text, bool invert);
        void _InitRender(IRenderer& renderer, Color4f const& color, std::string const& text, bool invert);
        void _RealRender(IRenderer& renderer, Color4f const& color, std::string const& text, bool invert);
    };

}}

#endif
