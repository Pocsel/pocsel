#include "tools/precompiled.hpp"

#include "tools/renderers/utils/material/Material.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    void Material::Effect::UpdateParameters(Uint64 totalTime)
    {
        for (auto it = this->scalars.begin(), ite = this->scalars.end(); it != ite; ++it)
            it->first->Set(it->second);
        for (auto it = this->vectors2.begin(), ite = this->vectors2.end(); it != ite; ++it)
            it->first->Set(it->second);
        for (auto it = this->vectors3.begin(), ite = this->vectors3.end(); it != ite; ++it)
            it->first->Set(it->second);
        for (auto it = this->vectors4.begin(), ite = this->vectors4.end(); it != ite; ++it)
            it->first->Set(it->second);
        for (auto it = this->textures.begin(), ite = this->textures.end(); it != ite; ++it)
            it->first->Set(it->second->GetCurrentTexture());
        this->totalTime->Set((float)totalTime * 0.000001f);
    }

    Material::Material(IRenderer& renderer, IShaderProgram& geometry, IShaderProgram& shadowMap) :
        _renderer(renderer),
        _geometry(geometry),
        _shadowMap(shadowMap)
    {
    }

    void Material::AddConstant(std::string const& name, float value)
    {
        auto param = this->_geometry.shader.GetParameter(name);
        if (param != 0)
            this->_geometry.scalars.push_back(std::make_pair(std::move(param), value));
        param = this->_shadowMap.shader.GetParameter(name);
        if (param != 0)
            this->_shadowMap.scalars.push_back(std::make_pair(std::move(param), value));
    }

    void Material::AddConstant(std::string const& name, glm::vec2 const& value)
    {
        auto param = this->_geometry.shader.GetParameter(name);
        if (param != 0)
            this->_geometry.vectors2.push_back(std::make_pair(std::move(param), value));
        param = this->_shadowMap.shader.GetParameter(name);
        if (param != 0)
            this->_shadowMap.vectors2.push_back(std::make_pair(std::move(param), value));
    }

    void Material::AddConstant(std::string const& name, glm::vec3 const& value)
    {
        auto param = this->_geometry.shader.GetParameter(name);
        if (param != 0)
            this->_geometry.vectors3.push_back(std::make_pair(std::move(param), value));
        param = this->_shadowMap.shader.GetParameter(name);
        if (param != 0)
            this->_shadowMap.vectors3.push_back(std::make_pair(std::move(param), value));
    }

    void Material::AddConstant(std::string const& name, glm::vec4 const& value)
    {
        auto param = this->_geometry.shader.GetParameter(name);
        if (param != 0)
            this->_geometry.vectors4.push_back(std::make_pair(std::move(param), value));
        param = this->_shadowMap.shader.GetParameter(name);
        if (param != 0)
            this->_shadowMap.vectors4.push_back(std::make_pair(std::move(param), value));
    }

    void Material::AddTexture(std::string const& name, std::unique_ptr<Texture::ITexture>&& texture)
    {
        auto param = this->_geometry.shader.GetParameter(name);
        bool inUse = param != 0;
        if (param != 0)
            this->_geometry.textures.push_back(std::make_pair(std::move(param), texture.get()));
        param = this->_shadowMap.shader.GetParameter(name);
        inUse = inUse || param != 0;
        if (param != 0)
            this->_shadowMap.textures.push_back(std::make_pair(std::move(param), texture.get()));

        if (inUse)
            this->_textures.push_back(std::move(texture));
    }

    void Material::SetTimeParameter(std::string const& name)
    {
        this->_geometry.totalTime = this->_geometry.shader.GetParameter(name);
        this->_shadowMap.totalTime = this->_shadowMap.shader.GetParameter(name);
    }

    void Material::Update(Uint64 totalTime)
    {
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            (*it)->Update(totalTime);
    }

    void Material::RenderGeometry(std::function<void()>& render, Uint64 totalTime)
    {
        do
        {
            this->_geometry.shader.BeginPass();
            this->_geometry.UpdateParameters(totalTime);
            for (auto it = this->_variables.begin(), ite = this->_variables.end(); it != ite; ++it)
                (*it)->UpdateParameter(0);
            render();
        } while (this->_geometry.shader.EndPass());
    }

    void Material::RenderShadowMap(std::function<void()>& render, Uint64 totalTime)
    {
        do
        {
            this->_shadowMap.shader.BeginPass();
            this->_shadowMap.UpdateParameters(totalTime);
            for (auto it = this->_variables.begin(), ite = this->_variables.end(); it != ite; ++it)
                (*it)->UpdateParameter(1);
            render();
        } while (this->_shadowMap.shader.EndPass());
    }

}}}}
