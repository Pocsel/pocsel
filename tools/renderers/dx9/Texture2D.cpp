#ifdef _WIN32
#include "tools/precompiled.hpp"
#include <IL/il.h>

#include "tools/renderers/DX9Renderer.hpp"

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/Texture2D.hpp"


namespace Tools { namespace Renderers { namespace DX9 {

    static int nbBindedTexture = 0;

    Texture2D::Texture2D(DX9Renderer& renderer, PixelFormat::Type format, Uint32 size, void const* data, Vector2u const& imgSize, void const* mipmapData) :
        _renderer(renderer),
        _hasAlpha(false),
        _texture(0)
    {
        if (format == PixelFormat::Png)
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
        else
        {
            this->_size = imgSize;
            //this->_FinishLoading(GetInternalFormatFromPixelFormat(format), GetFormatFromPixelFormat(format), data, (format >> 24) & 0xFF, mipmapData);
        }
    }

    Texture2D::Texture2D(DX9Renderer& renderer, std::string const& imagePath) :
        _renderer(renderer),
        _hasAlpha(false),
        _texture(0)
    {
        ILuint ilID;
        ilGenImages(1, &ilID);
        ilBindImage(ilID);
        if (!ilLoad(IL_PNG, imagePath.c_str()) && !ilLoad(IL_TYPE_UNKNOWN, imagePath.c_str()))
        {
            ilBindImage(0);
            ilDeleteImage(ilID);
            throw std::runtime_error("Texture2D::ctor: Can't load image.");
        }
        this->_FinishLoading(ilID);
    }

    void Texture2D::_FinishLoading(unsigned int ilID)
    {
        this->_size = Vector2u(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

        int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
        if (bytesPerPixel != 3 && bytesPerPixel != 4)
        {
            ilBindImage(0);
            ilDeleteImage(ilID);
            throw std::runtime_error("A texture must be 24 or 32 bits per pixels.");
        }

        unsigned int size = this->_size.w * this->_size.h;
        auto pixmap = new Color4<Uint8>[size];
        ilCopyPixels(0, 0, 0, this->_size.w, this->_size.h, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);

        ilBindImage(0);
        ilDeleteImage(ilID);

        this->_FinishLoading(D3DFMT_A8R8G8B8, pixmap, size, 0);
        delete [] pixmap;
    }

    void Texture2D::_FinishLoading(D3DFORMAT format, Color4<Uint8> const* data, std::size_t size, void const* mipmapData)
    {
        DXCHECKERROR(this->_renderer.GetDevice()->CreateTexture(this->_size.w, this->_size.h, 1, D3DUSAGE_DYNAMIC, format, D3DPOOL_DEFAULT, &this->_texture, 0));
        D3DLOCKED_RECT lockRect;
        DXCHECKERROR(this->_texture->LockRect(0, &lockRect, 0, D3DLOCK_DISCARD));
        Uint8* ptr = reinterpret_cast<Uint8*>(lockRect.pBits);
        for (int i = 0, j = 0; i < size / 4; ++i, j += 4)
        {
            this->_hasAlpha = this->_hasAlpha || (data[i].a != 255);
            ptr[j + 0] = data[i].a;
            ptr[j + 1] = data[i].r;
            ptr[j + 2] = data[i].g;
            ptr[j + 3] = data[i].b;
        }
        DXCHECKERROR(this->_texture->UnlockRect(0));

        if (false && mipmapData != 0)
        {
            int level = 1;
            Color4<Uint8> const* idx = reinterpret_cast<Color4<Uint8> const*>(mipmapData);
            for (Tools::Vector2u vsize = this->_size / 2; vsize.w >= 1 && vsize.h >= 1; vsize /= 2)
            {
                DXCHECKERROR(this->_texture->LockRect(level, &lockRect, 0, D3DLOCK_DISCARD));
                Uint8* ptr = reinterpret_cast<Uint8*>(lockRect.pBits);
                for (int i = 0; i < vsize.w*vsize.h; ++i)
                {
                    this->_hasAlpha = this->_hasAlpha || (data[i].a != 255);
                    ptr[i*4 + 0] = idx[i].a;
                    ptr[i*4 + 1] = idx[i].r;
                    ptr[i*4 + 2] = idx[i].g;
                    ptr[i*4 + 3] = idx[i].b;
                }
                DXCHECKERROR(this->_texture->UnlockRect(level));
                ++level;
                idx += vsize.w * vsize.h * 4;
            }
        }
    }

    Texture2D::~Texture2D()
    {
        if (this->_texture)
            this->_texture->Release();
    }

    void Texture2D::Bind()
    {
        this->_renderer.GetDevice()->SetTexture(0, this->_texture);
    }

    void Texture2D::Unbind()
    {
    }

    void Texture2D::SetFilters(TextureFilter::Type minFilter, TextureFilter::Type magFilter)
    {
        //GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilter(minFilter)));
        //GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilter(magFilter)));
    }

}}}

#endif
