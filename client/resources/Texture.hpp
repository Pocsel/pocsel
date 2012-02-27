#ifndef __CLIENT_RESOURCES_TEXTURE_HPP__
#define __CLIENT_RESOURCES_TEXTURE_HPP__

#include "tools/IRenderer.hpp"

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Resources {

    class Texture
    {
    private:
        std::vector<std::unique_ptr<Tools::Renderers::ITexture2D>> _frames;
        Uint64 _timePerFrame;
        int _currentFrame;
        Uint64 _lastFrame;
        bool _animated;

    public:
        Texture(Game::Game& game, Uint32 pluginId, std::string const& description);
        ~Texture();

        void Update(Uint64 totalTime); // totalTime in microsecond
        void Bind();
        void Unbind();

        Tools::Renderers::ITexture2D& GetCurrentTexture() { return *(this->_frames[this->_currentFrame]); }
        Tools::Renderers::ITexture2D const& GetCurrentTexture() const { return *(this->_frames[this->_currentFrame]); }
        bool HasAlpha() const { return this->GetCurrentTexture().HasAlpha(); }
    private:
        void _InitializeFrames(Tools::IRenderer& renderer, void const* data, std::size_t dataSize);
    };

}}

#endif
