#ifndef __CLIENT_RESOURCES_ANIMATEDTEXTURE_HPP__
#define __CLIENT_RESOURCES_ANIMATEDTEXTURE_HPP__

#include "tools/IRenderer.hpp"
#include "client/resources/ITexture.hpp"

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Resources {

    class AnimatedTexture : public ITexture
    {
    private:
        std::vector<std::unique_ptr<Tools::Renderers::ITexture2D>> _frames;
        Uint64 _timePerFrame;
        int _currentFrame;
        Uint64 _lastFrame;
        bool _animated;

    public:
        AnimatedTexture(Game::Game& game, Uint32 pluginId, std::string const& description);
        virtual ~AnimatedTexture() {}

        virtual void Update(Uint64 totalTime); // totalTime in microsecond
        virtual void Bind();
        virtual void Unbind();

        virtual Tools::Renderers::ITexture2D& GetCurrentTexture() { return *(this->_frames[this->_currentFrame]); }
        virtual Tools::Renderers::ITexture2D const& GetCurrentTexture() const { return *(this->_frames[this->_currentFrame]); }
        virtual bool HasAlpha() const { return this->GetCurrentTexture().HasAlpha(); }
    private:
        void _InitializeFrames(Tools::IRenderer& renderer, void const* data, std::size_t dataSize);
    };

}}

#endif
