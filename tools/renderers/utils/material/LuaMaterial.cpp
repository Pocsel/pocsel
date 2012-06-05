#include "tools/precompiled.hpp"

#include "tools/lua/MetaTable.hpp"
#include "tools/renderers/utils/material/LuaMaterial.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    namespace {
        template<class T>
        void SetValue(LuaMaterial& material, std::string const& key, Lua::Ref& value)
        {
            auto it = material._variables.find(key);

            Variable<T>* shaderVariable;
            if (it == material._variables.end())
                shaderVariable = &material._material->GetVariable<T>(key);
            else
                shaderVariable = reinterpret_cast<Variable<T>*>(it->second);
            shaderVariable->Set(value.Check<T>());
        }
    }

    LuaMaterial::LuaMaterial(IRenderer& renderer, Lua::Ref const& registeredMaterial, std::function<IShaderProgram&(std::string const&)> const& loadShader) :
        _type(registeredMaterial),
        _self(registeredMaterial.GetState().MakeTable())
    {
        this->_self.SetMetaTable(this->_type);
        auto& geometryShader = loadShader(this->_type["geometryShader"].Check<std::string>());
        auto& shadowMapShader = loadShader(this->_type["shadowMapShader"].Check<std::string>());
        this->_material.reset(new Material(renderer, this->_self, geometryShader, shadowMapShader));
    }

    LuaMaterial::LuaMaterial(LuaMaterial const& material) :
        _material(new Material(*material._material)),
        _type(material._type),
        _self(_type.GetState().MakeTable())
    {
        this->_self.SetMetaTable(this->_type);
    }

    LuaMaterial& LuaMaterial::operator =(LuaMaterial const& material)
    {
    }

    void LuaMaterial::LoadLuaTypes(Lua::Interpreter& interpreter)
    {
        auto table = Lua::MetaTable::Create<LuaMaterial>(interpreter);
        table.SetMetaMethod(Lua::MetaTable::NewIndex,
            [](Lua::CallHelper& helper)
            {
                auto& material = *helper.PopArg().Check<LuaMaterial*>();
                auto key = helper.PopArg().Check<std::string>();
                auto value = helper.PopArg();

                if (value.IsNumber())
                    material._SetValue(key, (float)value.CheckNumber());
                else if (value.Is<Texture::ITexture>())
                    material._SetValue(key, value.Check<Texture::ITexture*>());
            });
    }

}}}}
