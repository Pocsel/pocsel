#ifndef __TOOLS_RENDERERS_UTILS_TEXTUREATLAS_HPP__
#define __TOOLS_RENDERERS_UTILS_TEXTUREATLAS_HPP__

#include "tools/IRenderer.hpp"
#include "tools/Vector3.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class AnimatedTexture
    {
    private:
        std::vector<std::unique_ptr<ITexture2D>> _frames;
        int _currentFrame;

    public:
        AnimatedTexture(IRenderer& renderer, std::size_t dataSize, void const* data);
        ~AnimatedTexture();

        void Update(Uint32 elapsedTime); // elasped time in milliseconds
        void Bind();
        void Unbind();

        ITexture2D& GetCurrentFrameTexture() { return *(this->_frames[this->_currentFrame]); }
        bool HasAlpha() const
        {
            return this->_frame[0]->HasAlpha();
        }
    };

}}}

#endif
