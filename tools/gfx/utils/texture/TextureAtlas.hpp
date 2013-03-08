#pragma once

#include "tools/gfx/IRenderer.hpp"
#include "tools/Vector3.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Texture {

    class TextureAtlas
    {
    private:
        std::map<Uint32, glm::fvec3> _textureCoords;
        std::map<Uint32, bool> _hasAlpha;
        std::unique_ptr<ITexture2D> _texture;

    public:
        TextureAtlas(IRenderer& renderer, std::map<Uint32, std::pair<std::size_t, void const*>> const& textures);

        ITexture2D& GetTexture() { return *this->_texture; }
        glm::fvec3 const& GetCoords(Uint32 id) const
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

}}}}
