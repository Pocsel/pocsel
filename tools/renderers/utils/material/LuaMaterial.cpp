#include "tools/precompiled.hpp"

#include "tools/lua/Iterator.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/renderers/utils/material/LuaMaterial.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    LuaMaterial::LuaMaterial(IRenderer& renderer, Lua::Ref const& registeredMaterial, std::function<IShaderProgram&(std::string const&)> const& loadShader, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture) :
        _type(registeredMaterial),
        _self(registeredMaterial.GetState().MakeTable()),
        _loadTexture(loadTexture)
    {
        auto& geometryShader = loadShader(this->_type["geometryShader"].Check<std::string>());
        auto& shadowMapShader = loadShader(this->_type["shadowMapShader"].Check<std::string>());
        this->_material.reset(new Material(renderer, this->_self, geometryShader, shadowMapShader));

        this->_self.SetMetaTable(this->_type);
        this->_self.Set("shader", this->_self.GetState().GetInterpreter().Make<LuaMaterial*>(this));
        this->_LoadVariables();
    }

    LuaMaterial::LuaMaterial(LuaMaterial const& material) :
        _material(new Material(*material._material)),
        _type(material._type),
        _self(_type.GetState().MakeTable()),
        _loadTexture(material._loadTexture)
    {
        this->_self.SetMetaTable(this->_type);
        this->_self.Set("shader", this->_self.GetState().GetInterpreter().Make<LuaMaterial*>(this));
        this->_material->SetLuaMaterial(this->_self);
        this->_LoadVariables();
    }

    LuaMaterial& LuaMaterial::operator =(LuaMaterial const& material)
    {
        if (&material != this)
        {
            this->_material.reset(new Material(*material._material));
            this->_type = material._type;
            this->_self = this->_type.GetState().MakeTable();
            this->_loadTexture = material._loadTexture;
            this->_self.SetMetaTable(this->_type);
            this->_self.Set("shader", this->_self.GetState().GetInterpreter().Make<LuaMaterial*>(this));
            this->_material->SetLuaMaterial(this->_self);
            this->_LoadVariables();
        }
        return *this;
    }

    void LuaMaterial::LoadLuaTypes(Lua::Interpreter& interpreter, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture)
    {
        // Register type ITexture
        {
            // La métatable sert juste comme conteneur (du moins pour le moment)
            Lua::MetaTable::Create<std::shared_ptr<Texture::ITexture>>(interpreter);

            auto textureNs = interpreter.Globals().GetTable("Client").GetTable("Texture");
            auto metaTable = interpreter.MakeTable();
            metaTable.Set("__call", interpreter.MakeFunction(
                [loadTexture, &interpreter](Lua::CallHelper& helper)
                {
                    auto texture = loadTexture(helper.PopArg().CheckString());
                    helper.PushRet(interpreter.Make(std::shared_ptr<Texture::ITexture>(texture.release())));
                }));
            textureNs.SetMetaTable(metaTable);
        }

        // Register type LuaMaterial
        {
            auto table = Lua::MetaTable::Create<LuaMaterial*>(interpreter);
            table.SetMetaMethod(Lua::MetaTable::NewIndex,
                [](Lua::CallHelper& helper)
                {
                    auto material = *helper.PopArg().Check<LuaMaterial**>();
                    auto key = helper.PopArg().Check<std::string>();
                    auto value = helper.PopArg();

                    material->_SetLuaValue(key, value);
                });
            table.SetMetaMethod(Lua::MetaTable::Index,
                [](Lua::CallHelper& helper)
                {
                    auto material = *helper.PopArg().Check<LuaMaterial**>();
                    auto key = helper.PopArg().Check<std::string>();

                    auto it = material->_variables.find(key);
                    if (it == material->_variables.end())
                        return;

                    // TODO: Les autres types
                    if (auto value = dynamic_cast<Variable<float>*>(it->second)) // dynamic_cast c'est lent mais ça marche
                        helper.PushRet(helper.GetInterpreter().MakeNumber(value->Get()));
                    else if (auto value = dynamic_cast<Variable<Texture::ITexture>*>(it->second))
                        helper.PushRet(helper.GetInterpreter().Make(&value->Get()));
                    else
                        helper.PushRet(helper.GetInterpreter().MakeNil());
                });
        }
    }

    void LuaMaterial::_LoadVariables()
    {
        auto it = this->_type["shader"].Begin();
        auto ite = this->_type["shader"].End();
        for (; it != ite; ++it)
        {
            auto key = it.GetKey().CheckString();
            this->_SetLuaValue(key, it.GetValue());
        }
        auto update = this->_type["Update"];
        if (update.IsFunction())
            this->_material->SetLuaUpdate(update);
    }

    void LuaMaterial::_SetLuaValue(std::string const& key, Lua::Ref value)
    {
        // TODO: Les autres types
        if (value.IsNumber())
            this->_SetValue(key, (float)value.CheckNumber());
        else if (value.IsString())
            this->_SetValue(key, std::shared_ptr<Texture::ITexture>(this->_loadTexture(value.CheckString()).release()));
        else if (value.Is<std::shared_ptr<Texture::ITexture>>())
            this->_SetValue(key, *value.Check<std::shared_ptr<Texture::ITexture>*>());
    }

}}}}
