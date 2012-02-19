#ifndef __TOOLS_RENDERERS_UTILS_TEXTUREATLAS_HPP__
#define __TOOLS_RENDERERS_UTILS_TEXTUREATLAS_HPP__

#include "tools/IRenderer.hpp"
#include "tools/Vector3.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class TextureAtlas
    {
    private:
        std::map<Uint32, Vector3f> _textureCoords;
        std::map<Uint32, bool> _hasAlpha;
        ITexture2D* _texture;

    public:
        TextureAtlas(IRenderer& renderer, std::map<Uint32, std::pair<std::size_t, void const*>> const& textures);
        ~TextureAtlas();

        ITexture2D& GetTexture() { return *this->_texture; }
        Vector3f const& GetCoords(Uint32 id) const
        {
            auto it = this->_textureCoords.find(id);
            if (it == this->_textureCoords.end())
                throw std::runtime_error("");
            return it->second;
        }
        bool HasAlpha(Uint32 id) const
        {
            auto it = this->_hasAlpha.find(id);
            return (it != this->_hasAlpha.end() && it->second);
        }
    };

}}}

#endif
