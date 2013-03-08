#include "tools/precompiled.hpp"

#include "tools/gfx/effect/EffectManager.hpp"
#include "tools/gfx/effect/Shader.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    std::unique_ptr<Effect> EffectManager::CreateEffect(std::string const& path)
    {
        std::ifstream file(path);
        return EffectManager::CreateEffect(std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
    }

    std::unique_ptr<Effect> EffectManager::CreateEffect(std::vector<char> const& effect)
    {
        auto shader = CompleteShader(ByteArray(effect));
        return std::unique_ptr<Effect>(new Effect(this->_renderer, shader));
    }

}}}
