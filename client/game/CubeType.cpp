#include "client/precompiled.hpp"

#include "tools/lua/CallHelper.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/MetaTable.hpp"

#include "client/Client.hpp"
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

    void CubeType::Load(Resources::ResourceManager& resourceMgr, Tools::Lua::Interpreter& interpreter)
    {
        Uint32 pluginId = resourceMgr.GetPluginId(this->visualEffect);
        interpreter.DoString(resourceMgr.GetScript(this->visualEffect));

        auto textures = interpreter.Globals()["textures"];
        this->textures.top = resourceMgr.GetResourceId(pluginId, textures["top"].Check<std::string>());
        this->textures.bottom = resourceMgr.GetResourceId(pluginId, textures["bottom"].Check<std::string>());
        this->textures.right = resourceMgr.GetResourceId(pluginId, textures["right"].Check<std::string>());
        this->textures.left = resourceMgr.GetResourceId(pluginId, textures["left"].Check<std::string>());
        this->textures.front = resourceMgr.GetResourceId(pluginId, textures["front"].Check<std::string>());
        this->textures.back = resourceMgr.GetResourceId(pluginId, textures["back"].Check<std::string>());

        auto effects = interpreter.Globals()["effects"];
        this->effects.top = &resourceMgr.GetEffect(pluginId, effects["top"].Check<std::string>());
        this->effects.bottom = &resourceMgr.GetEffect(pluginId, effects["bottom"].Check<std::string>());
        this->effects.right = &resourceMgr.GetEffect(pluginId, effects["right"].Check<std::string>());
        this->effects.left = &resourceMgr.GetEffect(pluginId, effects["left"].Check<std::string>());
        this->effects.front = &resourceMgr.GetEffect(pluginId, effects["front"].Check<std::string>());
        this->effects.back = &resourceMgr.GetEffect(pluginId, effects["back"].Check<std::string>());

        // TODO !
        //auto effectParameters = interpreter.Globals()["effectParameters"];
        //this->effectParameters.top = _CreateEffectParameter(effectParameters["top"]);
        //this->effectParameters.bottom = _CreateEffectParameter(effectParameters["bottom"]);
        //this->effectParameters.right = _CreateEffectParameter(effectParameters["right"]);
        //this->effectParameters.left = _CreateEffectParameter(effectParameters["left"]);
        //this->effectParameters.front = _CreateEffectParameter(effectParameters["front"]);
        //this->effectParameters.back = _CreateEffectParameter(effectParameters["back"]);
    }

}}
