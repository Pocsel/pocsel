#include "tools/precompiled.hpp"

#include "tools/gfx/effect/EffectManager.hpp"
#include "tools/gfx/effect/Shader.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    std::unique_ptr<Effect> EffectManager::CreateEffect(std::string const& path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::in);
        return EffectManager::CreateEffect(std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
    }

    std::unique_ptr<Effect> EffectManager::CreateEffect(std::vector<char> const& effect)
    {
        return CreateEffect(ByteArray(effect));
    }

    std::unique_ptr<Effect> EffectManager::CreateEffect(ByteArray& effect)
    {
        auto shader = CompleteShader(effect);
        return std::unique_ptr<Effect>(new Effect(this->_renderer, shader));
    }

}}}
