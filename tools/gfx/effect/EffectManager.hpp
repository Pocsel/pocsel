#ifndef __TOOLS_RENDERERS_EFFECT_EFFECTMANAGER_HPP__
#define __TOOLS_RENDERERS_EFFECT_EFFECTMANAGER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/gfx/effect/Effect.hpp"
#include "tools/gfx/effect/RenderState.hpp"
#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    class EffectManager
    {
    private:
        IRenderer& _renderer;

    public:
        EffectManager(IRenderer& renderer) : _renderer(renderer) {}

        std::unique_ptr<Effect> CreateEffect(std::string const& path);
        std::unique_ptr<Effect> CreateEffect(std::vector<char> const& effect);
        std::unique_ptr<Effect> CreateEffect(ByteArray&& effect);
    };

}}}

#endif
