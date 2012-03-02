#include "tools/precompiled.hpp"
#include <IL/il.h>

#include "tools/renderers/GLRenderer.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/Texture2D.hpp"


namespace Tools { namespace Renderers { namespace OpenGL {

    static int nbBindedTexture = 0;

    Texture2D::Texture2D(GLRenderer& renderer, PixelFormat::Type format, Uint32 size, void const* data, Vector2u const& imgSize, void const* mipmapData) :
        _renderer(renderer),
        _hasAlpha(false)
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
            this->_FinishLoading(GetInternalFormatFromPixelFormat(format), GetFormatFromPixelFormat(format), data, (format >> 24) & 0xFF, mipmapData);
        }
    }

    Texture2D::Texture2D(GLRenderer& renderer, std::string const& imagePath) : _renderer(renderer), _hasAlpha(false)
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

        this->_FinishLoading(4, GL_RGBA, pixmap, 4, 0);
        delete [] pixmap;
    }

    void Texture2D::_FinishLoading(GLint internalFormat, GLenum format, GLvoid const* data, int pixelSize, void const* mipmapData)
    {
        GLCHECK(glGenTextures(1, &this->_id));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, this->_id));
        GLCHECK(glTexImage2D(GL_TEXTURE_2D,
                0,
                internalFormat,
                this->_size.w,
                this->_size.h,
                0,
                format,
                GL_UNSIGNED_BYTE,
                data));

        Color4<Uint8> const* pixmap = reinterpret_cast<Color4<Uint8> const*>(data);
        unsigned int size = this->_size.w * this->_size.h;
        for (unsigned int i = 0; i < size; ++i)
            if (pixmap[i].a != 255)
            {
                this->_hasAlpha = true;
                break;
            }

        GLCHECK(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE));

        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        if (mipmapData != 0)
        {
            int level = 1;
            char const* idx = (char const*)mipmapData;
            for (Tools::Vector2u size = this->_size / 2; size.w >= 1 && size.h >= 1; size /= 2)
            {
                GLCHECK(glTexImage2D(GL_TEXTURE_2D, level++, internalFormat, size.w, size.h, 0, format, GL_UNSIGNED_BYTE, idx));
                idx += size.w * size.h * pixelSize;
            }
        }
        else if (GLEW_VERSION_3_0)
            GLCHECK(glGenerateMipmap(GL_TEXTURE_2D));
        else
            gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, this->_size.w, this->_size.h, format, GL_UNSIGNED_BYTE, data); 

        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
    }

    Texture2D::~Texture2D()
    {
        GLCHECK(glDeleteTextures(1, &this->_id));
    }

    void Texture2D::Bind()
    {
        if (nbBindedTexture == 0)
            GLCHECK(glEnable(GL_TEXTURE_2D));
        this->_bindId = nbBindedTexture++;
        GLCHECK(glActiveTextureARB(GL_TEXTURE0 + this->_bindId));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, this->_id));
    }

    void Texture2D::Unbind()
    {
        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
        GLCHECK(glActiveTextureARB(GL_TEXTURE0 + this->_bindId));
        GLCHECK(glDisable(GL_TEXTURE_2D));

        this->_bindId = -1;
        --nbBindedTexture;
    }

    void Texture2D::SetFilters(TextureFilter::Type minFilter, TextureFilter::Type magFilter)
    {
        GLCHECK(glBindTexture(GL_TEXTURE_2D, this->_id));

        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilter(minFilter)));
        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilter(magFilter)));

        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
    }

}}}
