#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <cstring>

#include "tools/ToString.hpp"

#include "Font.hpp"

using namespace Tools;
using namespace Tools::Gui;

namespace {
    struct FTLibrary
    {
        ::FT_Library library;

        FTLibrary()
        {
            if (auto error = ::FT_Init_FreeType(&library))
                throw std::runtime_error("FT_Init_FreeType failed (error: " + ToString(error) + ")");
        }
        ~FTLibrary()
        {
            ::FT_Done_FreeType(this->library);
        }
    } ftlibrary;

    unsigned int NextPowerOfTwo(unsigned int n)
    {
        unsigned int i = 2;
        while (i < n)
            i = i * 2;
        return i;
    }
}

struct Font::FontData
{
    ::FT_Face face;

    FontData() : face(0)  {}
    ~FontData()
    {
        ::FT_Done_Face(this->face);
    }
};

void (*Font::_renderBase)(Font&, IRenderer& renderer, Color4f const&, std::string const&, bool) = &Font::_InitRenderBase;
Renderers::IShaderProgram* Font::_shader = 0;
Renderers::IShaderParameter* Font::_shaderTexture = 0;
Renderers::IShaderParameter* Font::_shaderColor = 0;
Renderers::IShaderParameter* Font::_shaderMultZ = 0;

Font::Font(std::string const& name, std::size_t fontSize) :
    _render(&Font::_InitRender),
    _data(new FontData())
{
    if (auto error = ::FT_New_Face(ftlibrary.library, name.c_str(), 0, &this->_data->face))
        throw std::runtime_error("FT_New_Face failed (error: " + ToString(error) + ")");
    ::FT_Set_Char_Size(this->_data->face, fontSize * 64, fontSize * 64, 96, 96);
}

Font::Font(void const* data, std::size_t dataLength, std::size_t fontSize) :
    _render(&Font::_InitRender),
    _data(new FontData())
{
    if (auto error = ::FT_New_Memory_Face(ftlibrary.library, (FT_Byte const*)data, dataLength, 0, &this->_data->face))
        throw std::runtime_error("FT_New_Face failed (error: " + ToString(error) + ")");
    ::FT_Set_Char_Size(this->_data->face, fontSize * 64, fontSize * 64, 96, 96);
}

Font::~Font()
{
    Delete(this->_data);
    Delete(this->_texture);
    Delete(this->_vertexBuffer);
}

// First init shader
void Font::_InitRenderBase(Font& font, IRenderer& renderer, Color4f const& color, std::string const& text, bool invert)
{
    // XXX init shader
    char const* vShader =
        "uniform float u_multZ;"
        ""
        "void main() {"
        "   gl_TexCoord[0] = gl_MultiTexCoord0;"
        "   vec4 pos = gl_Vertex;"
        "   pos.z *= u_multZ;"
        "   gl_Position = gl_ModelViewProjectionMatrix * pos;"
        "}";
    char const* fShader =
        "uniform sampler2D u_tex;"
        "uniform vec4 u_color;"
        ""
        "void main() {"
        "   vec4 color = texture2D(u_tex, gl_TexCoord[0].st);"
        "   color = color.rrrr * u_color.rgba;"
        "   gl_FragColor = color;"
        "}";

    Font::_shader = renderer.CreateProgram(vShader, fShader).release();
    Font::_shaderTexture = Font::_shader->GetParameter("u_tex").release();
    Font::_shaderColor = Font::_shader->GetParameter("u_color").release();
    Font::_shaderMultZ = Font::_shader->GetParameter("u_multZ").release();

    Font::_renderBase = &Font::_RenderBase;
    Font::_renderBase(font, renderer, color, text, invert);
}

// Bypass first init
void Font::_RenderBase(Font& font, IRenderer& renderer, Color4f const& color, std::string const& text, bool invert)
{
    font._Render(renderer, color, text, invert);
}

// Do render
void Font::_Render(IRenderer& renderer, Color4f const& color, std::string const& text, bool invert)
{
    (this->*_render)(renderer, color, text, invert);
}

// Second, init textures
void Font::_InitRender(IRenderer& renderer, Color4f const& color, std::string const& text, bool invert)
{
    // XXX init textures
    int totalWidth = 0;
    int maxHeight = 0;
    int maxHeightOffset = 0;

    // Calculate glyph informations
    this->_charInfos.resize(1 + Font::endChar - Font::startChar);
    for (int c = Font::startChar; c <= Font::endChar; c++)
    {
        auto char_index = ::FT_Get_Char_Index(this->_data->face, c);
        if (char_index == 0)
            char_index = ::FT_Get_Char_Index(this->_data->face, ' ');
        if (auto error = ::FT_Load_Glyph(this->_data->face, char_index, FT_LOAD_DEFAULT))
            throw std::runtime_error("FT_Load_Glyph failed (error: " + ToString(error) + ")");

        ::FT_Glyph glyph;
        if(auto error = ::FT_Get_Glyph(this->_data->face->glyph, &glyph))
            throw std::runtime_error("FT_Get_Glyph failed (error: " + ToString(error) + ")");
        ::FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
        ::FT_BitmapGlyph bitmap_glyph = (::FT_BitmapGlyph) glyph;
        ::FT_Bitmap& bitmap = bitmap_glyph->bitmap;

        totalWidth += bitmap_glyph->bitmap.width + 2;
        maxHeight = bitmap_glyph->bitmap.rows > maxHeight ? bitmap_glyph->bitmap.rows : maxHeight;
        maxHeightOffset = bitmap_glyph->top > maxHeightOffset ? bitmap_glyph->top : maxHeightOffset;

        ::FT_Done_Glyph(glyph);
    }

    int width = 512;
    int height = NextPowerOfTwo(maxHeight);

    // Fill the bitmap
    std::vector<Uint8> textureData(width * height);
#ifdef DEBUG
    std::memset(textureData.data(), 64, textureData.size());
#endif
    int rx = 0;
    int ry = 0;
    maxHeight = 0;
    for (int c = Font::startChar; c <= Font::endChar; c++)
    {
        auto char_index = ::FT_Get_Char_Index(this->_data->face, c);
        if (char_index == 0)
            char_index = ::FT_Get_Char_Index(this->_data->face, ' ');
        if (auto error = ::FT_Load_Glyph(this->_data->face, char_index, FT_LOAD_DEFAULT))
            throw std::runtime_error("FT_Load_Glyph failed (error: " + ToString(error) + ")");

        ::FT_Glyph glyph;
        if(auto error = ::FT_Get_Glyph(this->_data->face->glyph, &glyph))
            throw std::runtime_error("FT_Get_Glyph failed (error: " + ToString(error) + ")");
        ::FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
        ::FT_BitmapGlyph bitmap_glyph = (::FT_BitmapGlyph) glyph;
        ::FT_Bitmap& bitmap = bitmap_glyph->bitmap;

        CharInfo& info = this->_charInfos[c - Font::startChar];
        if (rx + bitmap.width + 2 > width)
        {
            rx = 0;
            ry += maxHeight + 2;
            maxHeight = 0;
        }

        if (ry + bitmap_glyph->bitmap.rows + 1 > height)
        {
            int oldHeight = height;
            while (ry + bitmap_glyph->bitmap.rows + 1 > height)
                height *= 2;
            textureData.resize(width * height);
#ifdef DEBUG
            std::memset(textureData.data() + width*oldHeight, 64, textureData.size() - width*oldHeight);
#endif
        }
        maxHeight = (bitmap_glyph->bitmap.rows > maxHeight ? bitmap_glyph->bitmap.rows : maxHeight);

        info.advance = Vector2f(this->_data->face->glyph->advance.x / 64.0f, bitmap_glyph->root.advance.y / 65536.0f);
        info.offset = Vector2f((float)bitmap_glyph->left, (float)maxHeightOffset - (float)bitmap_glyph->top);
        info.size = Vector2f((float)bitmap.width, (float)bitmap.rows);
        info.texUVmin = Vector2f((float)rx + 1, (float)ry + 1);
        info.texUVmax = Vector2f((float)rx + 1 + (float)bitmap.width, (float)bitmap.rows + ry + 1);

        for (int y = 0; y < bitmap.rows; ++y)
            for (int x = 0; x < bitmap.width; ++x)
                textureData[rx + x + 1 + (ry + y + 1) * width] = bitmap.buffer[x + y * bitmap.width];
        rx += bitmap.width + 2;

        ::FT_Done_Glyph(glyph);
    }

    this->_texture = renderer.CreateTexture2D(
        Renderers::PixelFormat::Luminance8,
        textureData.size(), textureData.data(),
        Vector2u(width, height)
    ).release();

    this->_vertexBuffer = renderer.CreateVertexBuffer().release();
    this->_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3);
    this->_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::TexCoord, 2);

    this->_render = &Font::_RealRender;
    (this->*_render)(renderer, color, text, invert);
}

namespace {

    void _InvertIndices(unsigned int count, unsigned short* tab)
    {
        for (unsigned int i = 0, inv = count-1; i < count; i += 3, count -= 3)
        {
            unsigned short tmp = tab[i];
            unsigned short tmp1 = tab[i + 1];
            unsigned short tmp2 = tab[i + 2];
            tab[i + 2] = tab[inv];
            tab[i + 1] = tab[inv - 1];
            tab[i] = tab[inv - 2];
            tab[inv - 2] = tmp;
            tab[inv - 1] = tmp1;
            tab[inv] = tmp2;
        }
    }

}
// Bypass init textures
void Font::_RealRender(IRenderer& renderer, Color4f const& color, std::string const& text, bool invert)
{
    float* vertices = new float[text.length() * 5 * 4];
    unsigned short* indices = new unsigned short[text.length() * 6];
    unsigned short* invertedIndices = 0;
    if (invert)
        invertedIndices = new unsigned short[text.length() * 6];
    int vi = 0;
    int ii = 0;
    int iiv = 0;
    unsigned short indiceValue = 0;
    float offsetX = 0;
    for (unsigned int i = 0; i < text.length(); ++i)
    {
        int c = text[i] & 0xff;
        if (c < Font::startChar || c > Font::endChar)
            c = '?';
        CharInfo& info = this->_charInfos[c - Font::startChar];
        if (info.size.w == 0)
        {
            offsetX += this->_charInfos[i].advance.x;
            continue;
        }

        vertices[vi++] = offsetX + info.offset.x; // X
        vertices[vi++] = info.offset.y; // Y
        vertices[vi++] = (i)*0.01f; // Z
        vertices[vi++] = info.texUVmin.x / this->_texture->GetSize().w; // U
        vertices[vi++] = info.texUVmin.y / this->_texture->GetSize().h; // V

        vertices[vi++] = offsetX + info.offset.x;
        vertices[vi++] = info.offset.y + info.size.h; // Y
        vertices[vi++] = (i)*0.01f; // Z
        vertices[vi++] = info.texUVmin.x / this->_texture->GetSize().w; // U
        vertices[vi++] = info.texUVmax.y / this->_texture->GetSize().h; // V

        vertices[vi++] = offsetX + info.offset.x + info.size.w; // X
        vertices[vi++] = info.offset.y; // Y
        vertices[vi++] = (i)*0.01f; // Z
        vertices[vi++] = info.texUVmax.x / this->_texture->GetSize().w; // U
        vertices[vi++] = info.texUVmin.y / this->_texture->GetSize().h; // V

        vertices[vi++] = offsetX + info.offset.x + info.size.w; // X
        vertices[vi++] = info.offset.y + info.size.h; // Y
        vertices[vi++] = (i)*0.01f; // Z
        vertices[vi++] = info.texUVmax.x / this->_texture->GetSize().w; // U
        vertices[vi++] = info.texUVmax.y / this->_texture->GetSize().h; // V

        if (invert)
        {
            invertedIndices[iiv++] = indiceValue;
            indices[ii++] = indiceValue + 1;
            invertedIndices[iiv++] = indiceValue + 2;
            indices[ii++] = indiceValue + 2;
            invertedIndices[iiv++] = indiceValue + 1;
            indices[ii++] = indiceValue;
            invertedIndices[iiv++] = indiceValue + 1;
            indices[ii++] = indiceValue + 3;
            invertedIndices[iiv++] = indiceValue + 2;
            indices[ii++] = indiceValue + 2;
            invertedIndices[iiv++] = indiceValue + 3;
            indices[ii++] = indiceValue + 1;
        }
        else
        {
            indices[ii++] = indiceValue + 1;
            indices[ii++] = indiceValue + 2;
            indices[ii++] = indiceValue;
            indices[ii++] = indiceValue + 3;
            indices[ii++] = indiceValue + 2;
            indices[ii++] = indiceValue + 1;
        }

        indiceValue += 4;
        offsetX += info.advance.x + info.offset.x;
    }

    this->_vertexBuffer->SetData(text.length() * 5 * 4 * sizeof(float), vertices, Renderers::VertexBufferUsage::Stream);
    this->_texture->Bind();
    this->_vertexBuffer->Bind();

    Font::_shader->Activate();
    Font::_shaderTexture->Set(*this->_texture);
    Font::_shaderColor->Set(color);
    Font::_shaderMultZ->Set(1.0f);
    renderer.DrawElements(indiceValue * 6 / 4, Renderers::DataType::UnsignedShort, indices);
    if (invert)
    {
        Font::_shaderMultZ->Set(-1.0f);
        _InvertIndices(indiceValue * 6 / 4, invertedIndices);
        renderer.DrawElements(indiceValue * 6 / 4, Renderers::DataType::UnsignedShort, invertedIndices);
    }
    this->_vertexBuffer->Unbind();
    this->_texture->Unbind();

    delete [] vertices;
    delete [] indices;
    delete [] invertedIndices;
}

#ifdef DEBUG

namespace {
    Renderers::IVertexBuffer& _CreateVB(IRenderer& renderer)
    {
        auto vb = renderer.CreateVertexBuffer().release();
        float vertices[4 * 5];
        int i = 0;
        //              X                  Y                  Z                  U                  V
        vertices[i++] = 1; vertices[i++] = 0; vertices[i++] = 0.001f; vertices[i++] = 1; vertices[i++] = 0;
        vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 0.001f; vertices[i++] = 0; vertices[i++] = 0;
        vertices[i++] = 1; vertices[i++] = 1; vertices[i++] = 0.001f; vertices[i++] = 1; vertices[i++] = 1;
        vertices[i++] = 0; vertices[i++] = 1; vertices[i++] = 0.001f; vertices[i++] = 0; vertices[i++] = 1;
        i = 0;
        vb->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3);
        vb->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::TexCoord, 2);
        vb->SetData(4 * 5 * sizeof(float), vertices, Renderers::VertexBufferUsage::Dynamic);
        return *vb;
    }
}

void Font::RenderTextureDebug(IRenderer& renderer)
{
    if (!this->_texture)
        return;
    static auto& quad = _CreateVB(renderer);

    Vector2f size((float)this->_texture->GetSize().x, (float)this->_texture->GetSize().y);
    quad.SetSubData(0*sizeof(float),  sizeof(float), &size.x);
    quad.SetSubData(10*sizeof(float), 2*sizeof(float), size.coords);
    quad.SetSubData(16*sizeof(float), sizeof(float), &size.y);

    this->_texture->Bind();
    quad.Bind();

    Font::_shader->Activate();
    Font::_shaderTexture->Set(*this->_texture);
    Font::_shaderColor->Set(Color4f(1, 1, 1, 1));
    Font::_shaderMultZ->Set(1.0f);
    renderer.DrawVertexBuffer(0, 4, Renderers::DrawingMode::TrianglesStrip);

    this->_texture->Unbind();
    quad.Unbind();
}

#endif
