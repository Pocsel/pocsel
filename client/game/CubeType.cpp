#include "client/precompiled.hpp"

#include "tools/lua/Function.hpp"

#include "client/game/CubeType.hpp"
#include "client/resources/Effect.hpp"
#include "client/resources/ResourceManager.hpp"

namespace Client { namespace Game {

    namespace {

        //std::unique_ptr<Resources::EffectState> _CreateEffectParameter(Tools::Lua::Ref parameters)
        //{

        //}

    }

    CubeType::CubeType(Common::CubeType const& cubeType)
        : Common::CubeType(cubeType)
    {
    }

    void CubeType::LoadMaterial(Resources::ResourceManager& resourceMgr, Tools::Lua::Ref const& prototype)
    {
        auto textures = prototype["textures"];
        this->textures.top = resourceMgr.GetResourceId(this->pluginId, textures["top"].Check<std::string>());
        this->textures.bottom = resourceMgr.GetResourceId(this->pluginId, textures["bottom"].Check<std::string>());
        this->textures.right = resourceMgr.GetResourceId(this->pluginId, textures["right"].Check<std::string>());
        this->textures.left = resourceMgr.GetResourceId(this->pluginId, textures["left"].Check<std::string>());
        this->textures.front = resourceMgr.GetResourceId(this->pluginId, textures["front"].Check<std::string>());
        this->textures.back = resourceMgr.GetResourceId(this->pluginId, textures["back"].Check<std::string>());

        auto effects = prototype["effects"];
        this->effects.top = &resourceMgr.GetEffect(this->pluginId, effects["top"].Check<std::string>());
        this->effects.bottom = &resourceMgr.GetEffect(this->pluginId, effects["bottom"].Check<std::string>());
        this->effects.right = &resourceMgr.GetEffect(this->pluginId, effects["right"].Check<std::string>());
        this->effects.left = &resourceMgr.GetEffect(this->pluginId, effects["left"].Check<std::string>());
        this->effects.front = &resourceMgr.GetEffect(this->pluginId, effects["front"].Check<std::string>());
        this->effects.back = &resourceMgr.GetEffect(this->pluginId, effects["back"].Check<std::string>());

        // TODO !
        //auto effectParameters = prototype["effectParameters"];
        //this->effectParameters.top = _CreateEffectParameter(effectParameters["top"]);
        //this->effectParameters.bottom = _CreateEffectParameter(effectParameters["bottom"]);
        //this->effectParameters.right = _CreateEffectParameter(effectParameters["right"]);
        //this->effectParameters.left = _CreateEffectParameter(effectParameters["left"]);
        //this->effectParameters.front = _CreateEffectParameter(effectParameters["front"]);
        //this->effectParameters.back = _CreateEffectParameter(effectParameters["back"]);
    }

}}
