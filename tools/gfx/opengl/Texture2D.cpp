#include "tools/precompiled.hpp"
#include <IL/il.h>
#include <IL/ilu.h>

#include "tools/gfx/GLRenderer.hpp"
#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/opengl/Texture2D.hpp"


namespace Tools { namespace Gfx { namespace OpenGL {

    static int nbBindedTexture = 0;

    Texture2D::Texture2D(GLRenderer& renderer, PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize, void const* mipmapData) :
        _renderer(renderer),
        _bindId(-1),
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
            this->_FinishLoading(GetInternalFormatFromPixelFormat(format), GetFormatFromPixelFormat(format), size, data, (format >> 24) & 0xFF, mipmapData);
        }
    }

    Texture2D::Texture2D(GLRenderer& renderer, std::string const& imagePath) :
        _renderer(renderer),
        _bindId(-1),
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

    Texture2D::Texture2D(GLRenderer& renderer, GLuint id) :
        _renderer(renderer),
        _id(id),
        _bindId(-1),
        _hasAlpha(false)
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
        iluFlipImage();
        ilCopyPixels(0, 0, 0, this->_size.x, this->_size.y, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);

        ilBindImage(0);
        ilDeleteImage(ilID);

        this->_FinishLoading(4, GL_RGBA, size, pixmap, 4, 0);
        delete [] pixmap;
    }

    void Texture2D::_FinishLoading(GLint internalFormat, GLenum format, Uint32 size, GLvoid const* data, int pixelSize, void const* mipmapData)
    {
        GLCHECK(glGenTextures(1, &this->_id));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, this->_id));
        GLCHECK(glTexImage2D(GL_TEXTURE_2D,
                0,
                internalFormat,
                this->_size.x,
                this->_size.y,
                0,
                format,
                GL_UNSIGNED_BYTE,
                data));

        glm::u8vec4 const* pixmap = reinterpret_cast<glm::u8vec4 const*>(data);
        for (unsigned int i = 0; i < size; ++i)
            if (pixmap[i].a != 255)
            {
                this->_hasAlpha = true;
                break;
            }

        //GLCHECK(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE));

        //GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        //GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        if (mipmapData != 0)
        {
            int level = 1;
            char const* idx = (char const*)mipmapData;
            for (glm::uvec2 size = this->_size / 2u; size.x >= 1 && size.y >= 1; size /= 2u)
            {
                GLCHECK(glTexImage2D(GL_TEXTURE_2D, level++, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, idx));
                idx += size.x * size.y * pixelSize;
            }
        }
        else if (GLEW_VERSION_3_0)
            GLCHECK(glGenerateMipmap(GL_TEXTURE_2D));
        else
            gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, this->_size.x, this->_size.y, format, GL_UNSIGNED_BYTE, data);

        //GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        //GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
    }

    Texture2D::~Texture2D()
    {
        GLCHECK(glDeleteTextures(1, &this->_id));
    }

    void Texture2D::Bind()
    {
        assert(this->_bindId == -1 && "already bind");
        this->_bindId = nbBindedTexture++;
        GLCHECK(glActiveTextureARB(GL_TEXTURE0 + this->_bindId));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, this->_id));
    }

    void Texture2D::Unbind()
    {
        GLCHECK(glActiveTextureARB(GL_TEXTURE0 + this->_bindId));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));

        this->_bindId = -1;
        --nbBindedTexture;
    }

    void Texture2D::SetFilters(TextureFilter::Type minFilter, TextureFilter::Type magFilter)
    {
        this->Bind();

        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilter(minFilter)));
        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilter(magFilter)));

        this->Unbind();
    }

}}}
