#ifndef __TOOLS_RENDERERS_UTILS_TEXTURE_ANIMATEDTEXTURE_HPP__
#define __TOOLS_RENDERERS_UTILS_TEXTURE_ANIMATEDTEXTURE_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Texture {

    class AnimatedTexture : public ITexture
    {
    private:
        std::vector<ITexture2D*> const* _frames;
        Uint64 _timePerFrame;
        int _currentFrame;
        Uint64 _lastFrame;
        bool _animated;

    public:
        AnimatedTexture(std::vector<ITexture2D*> const& frames, Uint64 timePerFrame, bool animated);
        virtual ~AnimatedTexture() {}

        virtual ITexture* Clone() { return new AnimatedTexture(*this); }

        virtual void Update(Uint64 totalTime); // totalTime in microsecond
        virtual void Bind();
        virtual void Unbind();

        virtual ITexture2D& GetCurrentTexture() { return *((*this->_frames)[this->_currentFrame]); }
        virtual ITexture2D const& GetCurrentTexture() const { return *((*this->_frames)[this->_currentFrame]); }
        virtual bool HasAlpha() const { return this->GetCurrentTexture().HasAlpha(); }
    };

}}}}

#endif
