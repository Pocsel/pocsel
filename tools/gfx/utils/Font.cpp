#include "tools/precompiled.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "tools/gfx/utils/Font.hpp"

namespace Tools { namespace Gfx { namespace Utils {

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

    Font::Font(IRenderer& renderer, std::string const& name, std::size_t fontSize)
        : _renderer(renderer),
        _data(new FontData())
    {
        if (auto error = ::FT_New_Face(ftlibrary.library, name.c_str(), 0, &this->_data->face))
            throw std::runtime_error("FT_New_Face failed (error: " + ToString(error) + ")");
        ::FT_Set_Char_Size(this->_data->face, static_cast<FT_F26Dot6>(fontSize * 64), static_cast<FT_F26Dot6>(fontSize * 64), 96, 96);
        this->_InitTextures();
    }

    Font::Font(IRenderer& renderer, void const* data, std::size_t dataLength, std::size_t fontSize)
        : _renderer(renderer),
        _data(new FontData())
    {
        if (auto error = ::FT_New_Memory_Face(ftlibrary.library, (FT_Byte const*)data, static_cast<FT_Long>(dataLength), 0, &this->_data->face))
            throw std::runtime_error("FT_New_Face failed (error: " + ToString(error) + ")");
        ::FT_Set_Char_Size(this->_data->face, static_cast<FT_F26Dot6>(fontSize * 64), static_cast<FT_F26Dot6>(fontSize * 64), 96, 96);
        this->_InitTextures();
    }

    Font::~Font()
    {
        Delete(this->_data);
    }

    // Init textures
    void Font::_InitTextures()
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

            totalWidth += bitmap_glyph->bitmap.width + 2;
            maxHeight = bitmap_glyph->bitmap.rows > maxHeight ? bitmap_glyph->bitmap.rows : maxHeight;
            maxHeightOffset = bitmap_glyph->top > maxHeightOffset ? bitmap_glyph->top : maxHeightOffset;

            ::FT_Done_Glyph(glyph);
        }

        unsigned int width = 1024;
        unsigned int height = NextPowerOfTwo(maxHeight);

        // Fill the bitmap
        std::vector<Uint8> textureData(width * height * 4);
#ifdef DEBUG
        std::memset(textureData.data(), 64, textureData.size());
#endif
        unsigned int rx = 0;
        unsigned int ry = 0;
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
#ifdef DEBUG
                auto oldSize = textureData.size();
#endif
                while (ry + bitmap_glyph->bitmap.rows + 1 > height)
                    height *= 2;
                textureData.resize(width * height * 4);
#ifdef DEBUG
                std::memset(textureData.data() + oldSize, 0xFF, textureData.size() - oldSize);
#endif
            }
            maxHeight = (bitmap_glyph->bitmap.rows > maxHeight ? bitmap_glyph->bitmap.rows : maxHeight);

            info.advance = glm::fvec2(this->_data->face->glyph->advance.x / 64.0f, bitmap_glyph->root.advance.y / 65536.0f);
            info.offset = glm::fvec2((float)bitmap_glyph->left, (float)maxHeightOffset - (float)bitmap_glyph->top);
            info.size = glm::fvec2((float)bitmap.width, (float)bitmap.rows);
            info.texUVmin = glm::fvec2((float)rx + 1, (float)ry + 1);
            info.texUVmax = glm::fvec2((float)rx + 1 + (float)bitmap.width, (float)bitmap.rows + ry + 1);

            for (int y = 0; y < bitmap.rows; ++y)
                for (int x = 0; x < bitmap.width; ++x)
                {
                    textureData[(rx + x + 1 + (ry + y + 1) * width) * 4 + 0] = bitmap.buffer[x + y * bitmap.width];
                    textureData[(rx + x + 1 + (ry + y + 1) * width) * 4 + 1] = bitmap.buffer[x + y * bitmap.width];
                    textureData[(rx + x + 1 + (ry + y + 1) * width) * 4 + 2] = bitmap.buffer[x + y * bitmap.width];
                    textureData[(rx + x + 1 + (ry + y + 1) * width) * 4 + 3] = bitmap.buffer[x + y * bitmap.width];
                }
            rx += bitmap.width + 2;

            ::FT_Done_Glyph(glyph);
        }

        this->_texture = this->_renderer.CreateTexture2D(
            PixelFormat::Rgba8,
            (Uint32)textureData.size(), textureData.data(),
            glm::uvec2(width, height));

        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3);
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::TexCoord0, 2);
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

    void Font::Render(IShaderParameter& textureParameter, std::string const& text, bool invert)
    {
        std::unique_ptr<float[]> vertices(new float[text.length() * 5 * 4]);
        std::unique_ptr<unsigned short[]> indices(new unsigned short[text.length() * 6]);
        std::unique_ptr<unsigned short[]> invertedIndices;
        if (invert)
            invertedIndices.reset(new unsigned short[text.length() * 6]);
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
            if (info.size.x == 0)
            {
                offsetX += this->_charInfos[i].advance.x;
                continue;
            }

            vertices[vi++] = offsetX + info.offset.x; // X
            vertices[vi++] = info.offset.y; // Y
            vertices[vi++] = (i)*0.01f; // Z
            vertices[vi++] = info.texUVmin.x / this->_texture->GetSize().x; // U
            vertices[vi++] = 1 - info.texUVmin.y / this->_texture->GetSize().y; // V

            vertices[vi++] = offsetX + info.offset.x;
            vertices[vi++] = info.offset.y + info.size.y; // Y
            vertices[vi++] = (i)*0.01f; // Z
            vertices[vi++] = info.texUVmin.x / this->_texture->GetSize().x; // U
            vertices[vi++] = 1 - info.texUVmax.y / this->_texture->GetSize().y; // V

            vertices[vi++] = offsetX + info.offset.x + info.size.x; // X
            vertices[vi++] = info.offset.y; // Y
            vertices[vi++] = (i)*0.01f; // Z
            vertices[vi++] = info.texUVmax.x / this->_texture->GetSize().x; // U
            vertices[vi++] = 1 - info.texUVmin.y / this->_texture->GetSize().y; // V

            vertices[vi++] = offsetX + info.offset.x + info.size.x; // X
            vertices[vi++] = info.offset.y + info.size.y; // Y
            vertices[vi++] = (i)*0.01f; // Z
            vertices[vi++] = info.texUVmax.x / this->_texture->GetSize().x; // U
            vertices[vi++] = 1 - info.texUVmax.y / this->_texture->GetSize().y; // V

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

        this->_vertexBuffer->SetData(text.length() * 4 * 5 * sizeof(float), vertices.get(), VertexBufferUsage::Stream);
        this->_texture->Bind();
        this->_vertexBuffer->Bind();

        textureParameter.Set(*this->_texture);

        this->_renderer.DrawElements(indiceValue * 6 / 4, DataType::UnsignedShort, indices.get());
        if (invert)
        {
            _InvertIndices(indiceValue * 6 / 4, invertedIndices.get());
            this->_renderer.DrawElements(indiceValue * 6 / 4, DataType::UnsignedShort, invertedIndices.get());
        }
        this->_vertexBuffer->Unbind();
        this->_texture->Unbind();
    }

}}}
