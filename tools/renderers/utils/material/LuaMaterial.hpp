#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_LUAMATERIAL_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_LUAMATERIAL_HPP__

#include "tools/IRenderer.hpp"
#include "tools/lua/Function.hpp"
#include "tools/renderers/utils/material/Material.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    class LuaMaterial
    {
    private:
        std::unique_ptr<Material> _material;
        Lua::Ref _type;
        Lua::Ref _self;
        std::map<std::string, IVariable*> _variables;
        std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> _loadTexture;

    public:
        LuaMaterial(IRenderer& renderer, Lua::Ref const& registedMaterial, std::function<IShaderProgram&(std::string const&)> const& loadShader, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture);

        explicit LuaMaterial(LuaMaterial const& material);
        LuaMaterial& operator =(LuaMaterial const& material);

        Material& GetMaterial() { return *this->_material; }

        static void LoadLuaTypes(Lua::Interpreter& interpreter, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture);
    private:
        void _LoadVariables();
        void _SetLuaValue(std::string const& key, Lua::Ref value);
        template<class T>
        void _SetValue(std::string const& key, T value)
        {
            auto it = this->_variables.find(key);

            Variable<T>* shaderVariable;
            if (it == this->_variables.end())
            {
                shaderVariable = &this->_material->GetVariable<T>(key);
                this->_variables[key] = shaderVariable;
            }
            else
                shaderVariable = reinterpret_cast<Variable<T>*>(it->second);
            shaderVariable->Set(value);
        }
        template<class T>
        void _SetValue(std::string const& key, std::shared_ptr<T> value)
        {
            auto it = this->_variables.find(key);

            Variable<T>* shaderVariable;
            if (it == this->_variables.end())
            {
                shaderVariable = &this->_material->GetVariable<T>(key);
                this->_variables[key] = shaderVariable;
            }
            else
                shaderVariable = reinterpret_cast<Variable<T>*>(it->second);
            shaderVariable->Set(value);
        }
    };

}}}}

#endif
