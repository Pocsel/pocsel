#include "tools/precompiled.hpp"
#include <IL/il.h>
#include <IL/ilu.h>

#include "tools/renderers/utils/AnimatedTexture.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    namespace {
        unsigned int NextPowerOfTwo(unsigned int n)
        {
            unsigned int i = 1;
            while (i < n)
                i *= 2;
            return i;
        }
    }

    AnimatedTexture::AnimatedTexture(IRenderer& renderer, std::size_t dataSize, void const* data)
        : _currentFrame(0)
    {
        ilDisable(IL_BLIT_BLEND);

        ILuint id = ilGenImage();
        ilBindImage(id);
        
        if (!ilLoadL(IL_PNG, data, (ILuint)dataSize) && !ilLoadL(IL_TYPE_UNKNOWN, data, (ILuint)dataSize))
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: Can't load data");
        }
        int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
        if (bytesPerPixel != 3 && bytesPerPixel != 4)
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: A texture must be 24 or 32 bits per pixels.");
        }

        auto size = Vector2u((ILuint)ilGetInteger(IL_IMAGE_WIDTH), (ILuint)ilGetInteger(IL_IMAGE_HEIGHT));
        if (size.w % size.h != 0)
        {
            ilBindImage(0);
            ilDeleteImage(id);
            throw std::runtime_error("AnimatedTexture: height must be a multiple of width.");
        }
        //maxSize.w = std::max(maxSize.w, size.w);
        //maxSize.h = std::max(maxSize.h, size.h);

        //textures[it->first] = id;
        //auto data = ilGetData();
        //this->_hasAlpha[it->first] = false;
        //for (unsigned int i = 3; i < size.w*size.h; i += 4)
        //    if (data[i] != 255)
        //    {
        //        this->_hasAlpha[it->first] = true;
        //        break;
        //    }

        //unsigned int tmp = (unsigned int)std::sqrt(textures.size()) + 1;
        //Vector2u totalSize(tmp * maxSize.w, tmp * maxSize.h);
        //ILuint result = ilGenImage();
        //ilBindImage(result);
        //ilTexImage(totalSize.w, totalSize.h, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, 0);

        //Vector2u pos(0);
        //for (auto it = textures.begin(), ite = textures.end(); it != ite; ++it)
        //{
        //    ilBindImage(it->second);
        //    iluScale(maxSize.w, maxSize.h, 1);
        //    //iluBuildMipmaps();

        //    ilBindImage(result);
        //    ilBlit(it->second,
        //        pos.x, pos.y, 0,
        //        0, 0, 0,
        //        maxSize.w, maxSize.h, 1);
        //    this->_textureCoords[it->first] = Vector3f((float)pos.x, (float)(totalSize.w - maxSize.w - pos.y), (float)maxSize.w) / (float)totalSize.w;

        //    pos.x += maxSize.w;
        //    if (pos.x >= totalSize.w)
        //    {
        //        pos.x = 0;
        //        pos.y += maxSize.h;
        //    }

        //    ilBindImage(0);
        //    ilDeleteImage(it->second);
        //}

        //ilBindImage(result);
        //unsigned int size = totalSize.w * totalSize.h;
        //auto pixmap = new Color4<Uint8>[size];
        //ilCopyPixels(0, 0, 0, totalSize.w, totalSize.h, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);
        //this->_texture = renderer.CreateTexture2D(PixelFormat::Rgba8, size, pixmap, totalSize, 0).release();
        //delete pixmap;
        //ilBindImage(0);
        //ilDeleteImage(result);
    }

    void AnimatedTexture::Update(Uint32 elapsedTime)
    {
    }

    void AnimatedTexture::Bind()
    {
    }

    void AnimatedTexture::Unbind()
    {
    }

}}}
