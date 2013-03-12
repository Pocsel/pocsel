#ifdef _WIN32
#include "tools/precompiled.hpp"
#include <IL/il.h>

#include "tools/gfx/DX9Renderer.hpp"
#include "tools/gfx/dx9/directx.hpp"
#include "tools/gfx/dx9/Texture2D.hpp"


namespace Tools { namespace Gfx { namespace DX9 {

    static int nbBindedTexture = 0;

    Texture2D::Texture2D(DX9Renderer& renderer, PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize, void const* mipmapData) :
        _renderer(renderer),
        _hasAlpha(false)
    {
        if (format == PixelFormat::Png && mipmapData == 0)
        {
            ILuint ilID;
            ilGenImages(1, &ilID);
            ilBindImage(ilID);
            if (!ilLoadL(IL_PNG, data, size) && !ilLoadL(IL_TYPE_UNKNOWN, data, size))
            {
                ilBindImage(0);
                ilDeleteImage(ilID);
                throw std::runtime_error("Texture2D::ctor: Can't load data.");
            }
            this->_FinishLoading(ilID);
        }
        else if (format == PixelFormat::Rgba8)
        {
            IDirect3DTexture9* tex;
            DXCHECKERROR(D3DXCreateTexture(this->_renderer.GetDevice(), imgSize.x, imgSize.y, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, &tex));
            this->_texture.reset(tex);
            this->_size = imgSize;
            this->_FinishLoading((glm::u8vec4 const*)data, size, mipmapData);
        }
        else
            throw std::runtime_error("Unsupported texture format");
    }

    Texture2D::Texture2D(DX9Renderer& renderer, std::string const& imagePath) :
        _renderer(renderer),
        _hasAlpha(false)
    {
            ILuint ilID;
            ilGenImages(1, &ilID);
            ilBindImage(ilID);
            if (!ilLoad(IL_PNG, imagePath.c_str()) && !ilLoad(IL_TYPE_UNKNOWN, imagePath.c_str()))
            {
                ilBindImage(0);
                ilDeleteImage(ilID);
                throw std::runtime_error("Texture2D::ctor: Can't load image file: " + imagePath);
            }
            this->_FinishLoading(ilID);
    }

    Texture2D::Texture2D(DX9Renderer& renderer, IDirect3DTexture9* texture) :
        _renderer(renderer),
        _hasAlpha(false),
        _texture(texture)
    {
    }

    void Texture2D::_FinishLoading(unsigned int ilID)
    {
        this->_size = glm::uvec2(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

        int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
        if (bytesPerPixel != 3 && bytesPerPixel != 4)
        {
            ilBindImage(0);
            ilDeleteImage(ilID);
            throw std::runtime_error("A texture must be 24 or 32 bits per pixels.");
        }

        unsigned int size = this->_size.x * this->_size.y;
        auto pixmap = new glm::u8vec4[size];
        ilCopyPixels(0, 0, 0, this->_size.x, this->_size.y, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);

        ilBindImage(0);
        ilDeleteImage(ilID);

        IDirect3DTexture9* tex;
        DXCHECKERROR(D3DXCreateTexture(this->_renderer.GetDevice(), this->_size.x, this->_size.y, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, &tex));
        this->_texture.reset(tex);
        this->_FinishLoading(pixmap, size, 0);
        delete [] pixmap;
    }

    void Texture2D::_FinishLoading(glm::u8vec4 const* data, std::size_t size, void const* mipmapData)
    {
        D3DLOCKED_RECT lockRect;
        DXCHECKERROR(this->_texture->LockRect(0, &lockRect, 0, 0));
        Uint8* ptr = reinterpret_cast<Uint8*>(lockRect.pBits);
        for (unsigned int y = 0; y < this->_size.y; ++y)
        {
            for (unsigned int x = 0; x < this->_size.x; ++x)
            {
                int i = y * this->_size.x + x;
                this->_hasAlpha = this->_hasAlpha || (data[i].a != 255);
                ptr[x * 4 + 0] = data[i].b;
                ptr[x * 4 + 1] = data[i].g;
                ptr[x * 4 + 2] = data[i].r;
                ptr[x * 4 + 3] = data[i].a;
            }
            ptr += lockRect.Pitch;
        }
        DXCHECKERROR(this->_texture->UnlockRect(0));

        if (mipmapData != 0)
        {
            int level = 1;
            glm::u8vec4 const* idx = reinterpret_cast<glm::u8vec4 const*>(mipmapData);
            for (auto vsize = glm::uvec2(this->_size.x / 2, this->_size.y / 2); vsize.x >= 1 && vsize.y >= 1; vsize /= 2)
            {
                DXCHECKERROR(this->_texture->LockRect(level, &lockRect, 0, D3DLOCK_DISCARD));
                Uint8* ptr = reinterpret_cast<Uint8*>(lockRect.pBits);
                for (unsigned int i = 0; i < vsize.x*vsize.y; ++i)
                {
                    ptr[i*4 + 0] = idx[i].b;
                    ptr[i*4 + 1] = idx[i].g;
                    ptr[i*4 + 2] = idx[i].r;
                    ptr[i*4 + 3] = idx[i].a;
                }
                DXCHECKERROR(this->_texture->UnlockRect(level));
                ++level;
                idx += vsize.x * vsize.y * 4;
            }
        }
        else
            this->_texture->GenerateMipSubLevels();
    }

    Texture2D::~Texture2D()
    {
        //if (this->_texture)
        //    this->_texture->Release();
    }

    void Texture2D::Bind()
    {
        this->_bindId = nbBindedTexture++;
        //this->_renderer.GetDevice()->SetTexture(this->_bindId, this->_texture.get());
    }

    void Texture2D::Unbind()
    {
        //this->_renderer.GetDevice()->SetTexture(this->_bindId, 0);
        nbBindedTexture--;
    }

    void Texture2D::SetFilters(TextureFilter::Type minFilter, TextureFilter::Type magFilter)
    {
        //GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilter(minFilter)));
        //GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilter(magFilter)));
    }

}}}

#endif
