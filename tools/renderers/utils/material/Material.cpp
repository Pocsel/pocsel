#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/renderers/utils/material/LuaMaterial.hpp"
#include "tools/renderers/utils/material/Material.hpp"
#include "tools/renderers/utils/material/Variable.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    void Material::Effect::UpdateParameters(Uint64 totalTime)
    {
        if (this->totalTime != 0)
            this->totalTime->Set((float)totalTime * 0.000001f);
    }

    Material::Material(IRenderer& renderer, Lua::Ref const& type, Lua::Ref& materialSelf, std::function<IShaderProgram&(std::string const&)> const& loadShader, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture) :
        _renderer(renderer),
        _loadTexture(loadTexture),
        _type(type),
        _self(materialSelf.GetState()),
        _hasAlpha(0)
    {
        materialSelf = materialSelf.GetState().MakeTable();
        this->_self = materialSelf;
        if (!materialSelf.IsNoneOrNil() && materialSelf["Update"].Exists())
            this->_luaUpdate.reset(new Lua::Ref(materialSelf["Update"]));

        auto& geometryShader = loadShader(this->_type["geometryShader"].Check<std::string>());
        this->_geometry.reset(new Effect(geometryShader));
        auto const& shadow = this->_type["shadowMapShader"].Check<std::string>();
        if (!shadow.empty())
            this->_shadowMap.reset(new Effect(loadShader(shadow)));
        this->_LoadVariables();

        materialSelf.SetMetaTable(this->_type);
    }

    Material::Material(Material const& material, Lua::Ref& materialSelf) :
        _renderer(material._renderer),
        _loadTexture(material._loadTexture),
        _geometry(new Effect(material._geometry->shader)),
        _type(material._type),
        _self(material._type.GetState()),
        _hasAlpha(0)
    {
        if (material._shadowMap != 0)
            this->_shadowMap.reset(new Effect(material._shadowMap->shader));
        materialSelf = materialSelf.GetState().MakeTable();
        this->_self = materialSelf;
        if (!materialSelf.IsNoneOrNil() && materialSelf["Update"].Exists())
            this->_luaUpdate.reset(new Lua::Ref(materialSelf["Update"]));
        this->_LoadVariables();

        materialSelf.SetMetaTable(this->_type);
    }

    void Material::SetTimeParameter(std::string const& name)
    {
        this->_geometry->totalTime = &this->_geometry->shader.GetParameter(name);
        this->_shadowMap->totalTime = &this->_shadowMap->shader.GetParameter(name);
    }

    void Material::SetLuaUpdate(Lua::Ref const& update)
    {
        this->_luaUpdate.release();
        if (update.Exists())
            this->_luaUpdate.reset(new Lua::Ref(update));
    }

    void Material::Update(Uint64 totalTime)
    {
        if (this->_luaUpdate != 0)
        {
            auto luaRef = this->_self.Lock();
            (*this->_luaUpdate)(luaRef, luaRef.GetState().MakeNumber((double)totalTime / 1000000.0));
        }
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            it->second->Update(totalTime);
    }

    void Material::RenderGeometry(std::function<void()>& render, Uint64 totalTime)
    {
        do
        {
            this->_geometry->shader.BeginPass();
            this->UpdateParameters(0, totalTime);
            for (auto it = this->_variables.begin(), ite = this->_variables.end(); it != ite; ++it)
                it->second->UpdateParameter(0);
            render();
        } while (this->_geometry->shader.EndPass());
    }

    void Material::RenderShadowMap(std::function<void()>& render, Uint64 totalTime)
    {
        if (this->_shadowMap == 0)
            return;
        do
        {
            this->_shadowMap->shader.BeginPass();
            this->UpdateParameters(1, totalTime);
            render();
        } while (this->_shadowMap->shader.EndPass());
    }

    void Material::UpdateParameters(int index, Uint64 totalTime)
    {
        if (index == 0)
            this->_geometry->UpdateParameters(totalTime);
        else if (this->_shadowMap != 0)
            this->_shadowMap->UpdateParameters(totalTime);
        for (auto it = this->_variables.begin(), ite = this->_variables.end(); it != ite; ++it)
            it->second->UpdateParameter(index);
    }

    void Material::UpdateParameters(IShaderProgram& shader, Uint64 totalTime)
    {
        this->UpdateParameters(&shader == &this->GetGeometryShader() ? 0 : 1, totalTime);
    }

    void Material::_LoadVariables()
    {
        auto it = this->_type["shader"].Begin();
        auto ite = this->_type["shader"].End();
        for (; it != ite; ++it)
        {
            auto key = it.GetKey().CheckString();
            this->SetLuaValue(key, it.GetValue());
        }
        auto update = this->_type["Update"];
        if (update.IsFunction())
            this->SetLuaUpdate(update);
        if (this->_type["hasAlpha"].IsBoolean())
            this->_hasAlpha = this->_type["hasAlpha"].CheckBoolean();
    }

    void Material::SetLuaValue(std::string const& key, Lua::Ref value)
    {
        // TODO: d'autres types ou pas ?
        if (value.IsNumber())
            this->_SetValue(key, (float)value.ToNumber());
        else if (value.Is<glm::vec2>())
            this->_SetValue(key, value.To<glm::vec2>());
        else if (value.Is<glm::vec3>())
            this->_SetValue(key, value.To<glm::vec3>());
        else if (value.Is<glm::vec4>())
            this->_SetValue(key, value.To<glm::vec4>());
        else if (value.Is<glm::mat4>())
            this->_SetValue(key, value.To<glm::mat4>());
        else if (value.Is<std::shared_ptr<Texture::ITexture>>())
            this->_SetValue(key, *value.Check<std::shared_ptr<Texture::ITexture>*>());
    }

    void Material::LoadLuaTypes(
        Lua::Interpreter& interpreter,
        std::function<std::unique_ptr<Texture::ITexture>(std::string const&)>&& loadTexture,
        std::function<std::unique_ptr<LuaMaterial>(std::string const&)>&& loadMaterial)
    {
        // Register type ITexture
        {
            // La métatable sert juste comme conteneur (du moins pour le moment)
            Lua::MetaTable::Create<std::shared_ptr<Texture::ITexture>>(interpreter);

            // Fonction: Client.Texture("plugin:NomDeTexture")
            auto textureNs = interpreter.Globals().GetTable("Client").GetTable("Texture");
            auto metaTable = interpreter.MakeTable();
            metaTable.Set("__call", interpreter.MakeFunction(
                [loadTexture, &interpreter](Lua::CallHelper& helper)
                {
                    helper.PopArg();
                    auto texture = loadTexture(helper.PopArg().CheckString());
                    helper.PushRet(std::shared_ptr<Texture::ITexture>(texture.release()));
                }));
            textureNs.SetMetaTable(metaTable);
        }

        // Register type LuaMaterial
        {
            // MetaTable pour le shader
            auto& table = Lua::MetaTable::Create<std::unique_ptr<Material>>(interpreter);
            table.SetMetaMethod(Lua::MetaTable::NewIndex,
                [](Lua::CallHelper& helper)
                {
                    auto material = helper.PopArg().To<std::unique_ptr<Material>*>()->get();
                    auto key = helper.PopArg().Check<std::string>();
                    auto value = helper.PopArg();

                    material->SetLuaValue(key, value);
                });
            table.SetMetaMethod(Lua::MetaTable::Index,
                [](Lua::CallHelper& helper)
                {
                    auto material = helper.PopArg().To<std::unique_ptr<Material>*>()->get();
                    auto key = helper.PopArg().Check<std::string>();

                    auto it = material->_variables.find(key);
                    if (it == material->_variables.end())
                        return;

                    helper.PushRet(it->second->GetLuaValue(helper.GetInterpreter()));
                });

            // Fonction: Client.Material("plugin:NomDeMaterial")
            auto materialNs = interpreter.Globals().GetTable("Client").GetTable("Material");
            auto metaTable = interpreter.MakeTable();
            metaTable.Set("__call", interpreter.MakeFunction(
                [loadMaterial, &interpreter](Lua::CallHelper& helper)
                {
                    helper.PopArg(); // Il y a un argument en plus, je ne sais pas trop quoi, ça print "[base/?]" en lua
                    auto material = loadMaterial(helper.PopArg().CheckString());
                    auto vars = helper.PopArg();
                    if (vars.IsTable())
                        for (auto it = vars.Begin(), ite = vars.End(); it != ite; ++it)
                            material->GetMaterial().SetLuaValue(it.GetKey().CheckString(), it.GetValue());
                    helper.PushRet(material->GetLuaSelf());
                }));
            materialNs.SetMetaTable(metaTable);
        }
    }

}}}}
