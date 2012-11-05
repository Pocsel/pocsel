#include "tools/precompiled.hpp"

#include <luasel/Luasel.hpp>

#include "tools/renderers/utils/light/LightRenderer.hpp"

#include "common/Camera.hpp"
#include "common/Position.hpp"

#include "client/Client.hpp"
#include "client/resources/LocalResourceManager.hpp"
#include "client/game/Game.hpp"
#include "client/graphics/LightManager.hpp"

using Tools::Renderers::Utils::Light::LightRenderer;

namespace Client { namespace Graphics {

    LightManager::DirectionnalLight::DirectionnalLight(LightManager& lightManager) :
        _lightManager(lightManager),
        _internal(lightManager._lightRenderer.CreateDirectionnalLight()),
        visible(true)
    {
    }

    LightManager::PointLight::PointLight(LightManager& lightManager) :
        _lightManager(lightManager),
        _internal(lightManager._lightRenderer.CreatePointLight()),
        position(0.0, 0.0, 0.0),
        visible(true)
    {
    }

    LightManager::LightManager(::Client::Game::Game& game) :
        _game(game),
        _renderer(game.GetRenderer()),
        _lightRenderer(_renderer, game.GetClient().GetLocalResourceManager().GetShader("DirectionnalLight.fx"), game.GetClient().GetLocalResourceManager().GetShader("PointLight.fx"))
    {
        this->_RegisterLua(game.GetInterpreter());
    }

    void LightManager::Render(Common::Camera const& camera, Tools::Renderers::Utils::GBuffer& gbuffer)
    {
        std::list<Tools::Renderers::Utils::Light::DirectionnalLight*> directionnals;
        for (auto it = this->_directionnals.begin(), ite = this->_directionnals.end(); it != ite; ++it)
            if ((*it)->visible)
                directionnals.push_back(&(*it)->GetInternal());
        std::list<Tools::Renderers::Utils::Light::PointLight*> points;
        for (auto it = this->_points.begin(), ite = this->_points.end(); it != ite; ++it)
            if ((*it)->visible)
            {
                auto& light = (*it)->GetInternal();
                light.position = glm::vec3((*it)->position - camera.position);
                points.push_back(&light);
            }

            std::function<void(glm::dmat4)> func = [](glm::dmat4 mat4){};
            this->_lightRenderer.Render(
                gbuffer,
                camera.GetViewMatrix(),
                camera.projection,
                Tools::Frustum(camera.GetAbsoluteViewProjectionMatrix()),
                camera.position,
                func,
                directionnals,
                points);
    }

    LightManager::DirectionnalLight& LightManager::CreateDirectionnalLight()
    {
        auto ptr = new DirectionnalLight(*this);
        this->_directionnals.push_back(std::unique_ptr<std::remove_reference<decltype(*ptr)>::type>(ptr));
        return *ptr;
    }

    LightManager::PointLight& LightManager::CreatePointLight()
    {
        auto ptr = new PointLight(*this);
        this->_points.push_back(std::unique_ptr<std::remove_reference<decltype(*ptr)>::type>(ptr));
        return *ptr;
    }

    void LightManager::_Unregister(LightManager::DirectionnalLight& light)
    {
        this->_directionnals.remove_if([&](std::unique_ptr<DirectionnalLight> const& ptr) { return ptr.get() == &light; });
    }

    void LightManager::_Unregister(LightManager::PointLight& light)
    {
        this->_points.remove_if([&](std::unique_ptr<PointLight> const& ptr) { return ptr.get() == &light; });
    }

    void LightManager::_RegisterLua(Luasel::Interpreter& interpreter)
    {
        auto ns = interpreter.Globals().GetTable("Client").GetTable("Light");

        auto& mt = Luasel::MetaTable::Create<LightManager::PointLight*>(interpreter);
        mt.SetMetaMethod(Luasel::MetaTable::Index,
            [](Luasel::CallHelper& helper)
            {
                auto light = *helper.PopArg().Check<LightManager::PointLight**>();
                auto const& key = helper.PopArg().Check<std::string>();
                if (key == "position")
                    helper.PushRet(light->position);
                else if (key == "range")
                    helper.PushRet(light->GetRange());
                else if (key == "diffuseColor")
                    helper.PushRet(light->GetDiffuseColor());
                else if (key == "specularColor")
                    helper.PushRet(light->GetSpecularColor());
            });
        mt.SetMetaMethod(Luasel::MetaTable::NewIndex,
            [](Luasel::CallHelper& helper)
            {
                auto light = *helper.PopArg().Check<LightManager::PointLight**>();
                auto const& key = helper.PopArg().Check<std::string>();
                if (key == "position")
                    light->position = helper.PopArg().Check<glm::vec3>();
                else if (key == "range")
                    light->SetRange(helper.PopArg().Check<float>());
                else if (key == "diffuseColor")
                    light->SetDiffuseColor(helper.PopArg().Check<glm::vec3>());
                else if (key == "specularColor")
                    light->SetSpecularColor(helper.PopArg().Check<glm::vec3>());
            });
        ns.Set("Point", interpreter.MakeFunction(
            [this](Luasel::CallHelper& helper)
            {
                auto& light = this->CreatePointLight();
                auto doodad = helper.PopArg();
                auto ac = helper.GetNbArgs();
                if (ac > 1)
                {
                    light.position = helper.PopArg().Check<glm::vec3>();
                    if (ac > 2)
                    {
                        light.SetRange(helper.PopArg().Check<float>());
                        if (ac > 3)
                        {
                            light.SetDiffuseColor(helper.PopArg().Check<glm::vec3>());
                            light.SetSpecularColor(ac > 4 ? helper.PopArg().Check<glm::vec3>() : light.GetDiffuseColor());
                        }
                    }
                }
                helper.PushRet(&light);
            }));
    }

}}
