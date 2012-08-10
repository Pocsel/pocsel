#ifndef __TOOLS_RENDERERS_EFFECT_EFFECTMANAGER_HPP__
#define __TOOLS_RENDERERS_EFFECT_EFFECTMANAGER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/IRenderer.hpp"
#include "tools/renderers/effect/Effect.hpp"
#include "tools/renderers/effect/RenderState.hpp"

namespace Tools { namespace Renderers { namespace Effect {

    class EffectManager
    {
    private:
        IRenderer& _renderer;

    public:
        EffectManager(IRenderer& renderer) : _renderer(renderer) {}

        std::unique_ptr<Effect> CreateEffect(std::vector<char> const& effect);
    };

    inline std::unique_ptr<Effect> EffectManager::CreateEffect(std::vector<char> const& effect)
    {
        ByteArray stream(effect);
        RenderState rs(stream);
        return std::unique_ptr<Effect>(new Effect(this->_renderer.CreateProgram(stream), rs));
    }

}}}

#endif
