#include "tools/precompiled.hpp"
#include <IL/il.h>
#include <IL/ilu.h>

#include "tools/gfx/utils/texture/TextureAtlas.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Texture {

    namespace {
        unsigned int NextPowerOfTwo(unsigned int n)
        {
            unsigned int i = 1;
            while (i < n)
                i *= 2;
            return i;
        }
    }

    TextureAtlas::TextureAtlas(IRenderer& renderer, std::map<Uint32, std::pair<std::size_t, void const*>> const& resources)
    {
        ilDisable(IL_BLIT_BLEND);
        glm::uvec2 maxSize(0);
        std::map<Uint32, ILuint> textures;
        for (auto it = resources.begin(), ite = resources.end(); it != ite; ++it)
        {
            ILuint id = ilGenImage();
            ilBindImage(id);
            if (it->second.first == 0)
            {
                unsigned char toto[] = {
                    0, 0, 0, 255,
                    255, 0, 255, 255,
                    255, 0, 255, 255,
                    0, 0, 0, 255
                };
                ilTexImage(2, 2, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, (void*)toto);
            }
            else
            {
                if (!ilLoadL(IL_PNG, it->second.second, (ILuint)it->second.first) && !ilLoadL(IL_TYPE_UNKNOWN, it->second.second, (ILuint)it->second.first))
                {
                    ilBindImage(0);
                    ilDeleteImage(id);
                    throw std::runtime_error("TextureAtlas: Can't load data (id: " + ToString(it->first) + ")");
                }
                int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
                if (bytesPerPixel != 4)
                {
                    ilBindImage(0);
                    ilDeleteImage(id);
                    throw std::runtime_error("TextureAtlas: A texture must be 32 bits per pixels (id: " + ToString(it->first) + ").");
                }
            }
            auto size = glm::uvec2((ILuint)ilGetInteger(IL_IMAGE_WIDTH), (ILuint)ilGetInteger(IL_IMAGE_HEIGHT));
            if (size.x != size.y || size.x != NextPowerOfTwo(size.x))
            {
                ilBindImage(0);
                ilDeleteImage(id);
                throw std::runtime_error("TextureAtlas: Dimensions must be power of two and square (id: " + ToString(it->first) + ").");
            }
            maxSize.x = std::max(maxSize.x, size.x);
            maxSize.y = std::max(maxSize.y, size.y);

            textures[it->first] = id;
            auto data = ilGetData();
            this->_hasAlpha[it->first] = false;
            for (unsigned int i = 3; i < size.x*size.y; i += 4)
                if (data[i] != 255)
                {
                    this->_hasAlpha[it->first] = true;
                    break;
                }
        }

        unsigned int tmp = (unsigned int)std::sqrt(textures.size()) + 1;
        glm::uvec2 totalSize(tmp * maxSize.x, tmp * maxSize.y);
        ILuint result = ilGenImage();
        ilBindImage(result);
        ilTexImage(totalSize.x, totalSize.y, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, 0);

        glm::uvec2 pos(0);
        for (auto it = textures.begin(), ite = textures.end(); it != ite; ++it)
        {
            ilBindImage(it->second);
            iluScale(maxSize.x, maxSize.y, 1);
            //iluBuildMipmaps();

            ilBindImage(result);
            ilBlit(it->second,
                pos.x, pos.y, 0,
                0, 0, 0,
                maxSize.x, maxSize.y, 1);
            this->_textureCoords[it->first] = glm::fvec3((float)pos.x, (float)(totalSize.x - maxSize.x - pos.y), (float)maxSize.x) / (float)totalSize.x;

            pos.x += maxSize.x;
            if (pos.x >= totalSize.x)
            {
                pos.x = 0;
                pos.y += maxSize.y;
            }

            ilBindImage(0);
            ilDeleteImage(it->second);
        }

        ilBindImage(result);
        unsigned int size = totalSize.x * totalSize.y;
        auto pixmap = new glm::u8vec4[size];
        ilCopyPixels(0, 0, 0, totalSize.x, totalSize.y, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);
        this->_texture = renderer.CreateTexture2D(PixelFormat::Rgba8, size, pixmap, totalSize, 0);
        delete [] pixmap;
        ilBindImage(0);
        ilDeleteImage(result);
    }

}}}}
