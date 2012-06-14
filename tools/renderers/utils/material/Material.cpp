#include "tools/precompiled.hpp"

#include "tools/renderers/utils/material/Material.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    void Material::Effect::UpdateParameters(Uint64 totalTime)
    {
        if (this->totalTime != 0)
            this->totalTime->Set((float)totalTime * 0.000001f);
    }

    Material::Material(IRenderer& renderer, Lua::Ref const& material, IShaderProgram& geometry, IShaderProgram& shadowMap) :
        _renderer(renderer),
        _geometry(geometry),
        _shadowMap(shadowMap),
        _luaMaterial(material)
    {
        if (!this->_luaMaterial.IsNoneOrNil() && this->_luaMaterial["Update"].Exists())
            this->_luaUpdate.reset(new Lua::Ref(this->_luaMaterial["Update"]));
    }

    Material::Material(Material const& material) :
        _renderer(material._renderer),
        _geometry(material._geometry.shader),
        _shadowMap(material._shadowMap.shader),
        _luaMaterial(material._luaMaterial)
    {
        if (!this->_luaMaterial.IsNoneOrNil() && this->_luaMaterial["Update"].Exists())
            this->_luaUpdate.reset(new Lua::Ref(this->_luaMaterial["Update"]));
    }

    void Material::SetTimeParameter(std::string const& name)
    {
        this->_geometry.totalTime = this->_geometry.shader.GetParameter(name);
        this->_shadowMap.totalTime = this->_shadowMap.shader.GetParameter(name);
    }

    void Material::SetLuaUpdate(Lua::Ref const& update)
    {
        this->_luaUpdate.release();
        if (update.Exists())
            this->_luaUpdate.reset(new Lua::Ref(update));
    }

    void Material::Update(Uint64 totalTime)
    {
        if (this->_luaUpdate != 0 && this->_luaUpdate->Exists())
            (*this->_luaUpdate)(totalTime);
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            it->second->Update(totalTime);
    }

    void Material::RenderGeometry(std::function<void()>& render, Uint64 totalTime)
    {
        do
        {
            this->_geometry.shader.BeginPass();
            this->UpdateParameters(0, totalTime);
            for (auto it = this->_variables.begin(), ite = this->_variables.end(); it != ite; ++it)
                it->second->UpdateParameter(0);
            render();
        } while (this->_geometry.shader.EndPass());
    }

    void Material::RenderShadowMap(std::function<void()>& render, Uint64 totalTime)
    {
        do
        {
            this->_shadowMap.shader.BeginPass();
            this->UpdateParameters(1, totalTime);
            render();
        } while (this->_shadowMap.shader.EndPass());
    }

    void Material::UpdateParameters(int index, Uint64 totalTime)
    {
        if (index == 0)
            this->_geometry.UpdateParameters(totalTime);
        else
            this->_shadowMap.UpdateParameters(totalTime);
        for (auto it = this->_variables.begin(), ite = this->_variables.end(); it != ite; ++it)
            it->second->UpdateParameter(index);
    }

    void Material::UpdateParameters(IShaderProgram& shader, Uint64 totalTime)
    {
        this->UpdateParameters(&shader == &this->GetGeometryShader() ? 0 : 1, totalTime);
    }

}}}}
