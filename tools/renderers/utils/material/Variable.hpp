#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_VARIABLE_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_VARIABLE_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/material/Material.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    template<class T>
    class Variable : public IVariable
    {
    private:
        T _value;
        std::vector<IShaderParameter*> _parameters;

    public:
        Variable(Material&, std::vector<IShaderParameter*>&& parameters)
        {
            this->_parameters.swap(parameters);
        }

        T Get() const
        {
            return this->_value;
        }

        void Set(T value)
        {
            this->_value = value;
        }

        virtual Lua::Ref GetLuaValue(Lua::Interpreter& interpreter) const
        {
            return interpreter.Make(this->_value);
        }

        virtual void UpdateParameter(int index)
        {
            if (this->_parameters[index] != 0)
                this->_parameters[index]->Set(this->_value);
        }
    };

    // Spécialisation pour les textures
    template<>
    class Variable<Texture::ITexture> : public IVariable
    {
    private:
        Material& _material;
        Texture::ITexture* _texture;
        std::vector<IShaderParameter*> _parameters;

    public:
        Variable(Material& material, std::vector<IShaderParameter*>&& parameters) :
            _material(material),
            _texture(0)
        {
            this->_parameters.swap(parameters);
        }

        Texture::ITexture& Get()
        {
            return *this->_texture;
        }

        void Set(std::shared_ptr<Texture::ITexture> texture)
        {
            this->_texture = texture.get();
            this->_material.SetTextures(*this, texture);
        }

        virtual Lua::Ref GetLuaValue(Lua::Interpreter& interpreter) const
        {
            return interpreter.Make(this->_texture);
        }

        virtual void UpdateParameter(int index)
        {
            if (this->_texture != 0)
            {
                auto ptr = this->_parameters[index];
                if (ptr != 0)
                    ptr->Set(this->_texture->GetCurrentTexture());
            }
        }
    };

    template<class TValue>
    inline Variable<TValue>& Material::GetVariable(std::string const& name)
    {
        auto it = this->_variables.find(name);
        if (it == this->_variables.end())
        {
            std::vector<IShaderParameter*> parameters;
            auto param = &this->_geometry->shader.GetParameter(name);
            parameters.push_back(param->IsUseable() ? param : 0);
            param = &this->_geometry->shader.GetParameter(name);
            parameters.push_back(param->IsUseable() ? param : 0);
            auto ptr = new Variable<TValue>(*this, std::move(parameters));
            this->_variables.insert(std::move(std::make_pair(name, std::unique_ptr<IVariable>(ptr))));
            return *ptr;
        }
        return reinterpret_cast<Variable<TValue>&>(*it->second);
    }

    template<class T>
    inline void Material::_SetValue(std::string const& key, T value)
    {
        //auto it = this->_variables.find(key);

        Variable<T>* shaderVariable;
        //if (it == this->_variables.end())
        //{
            shaderVariable = &this->GetVariable<T>(key);
            //this->_variables[key].reset(shaderVariable);
        //}
        //else
        //    shaderVariable = reinterpret_cast<Variable<T>*>(it->second.get());
        shaderVariable->Set(value);
    }

    template<class T>
    inline void Material::_SetValue(std::string const& key, std::shared_ptr<T> value)
    {
        //auto it = this->_variables.find(key);

        Variable<T>* shaderVariable;
        //if (it == this->_variables.end())
        //{
            shaderVariable = &this->GetVariable<T>(key);
            //this->_variables[key].reset(shaderVariable);
        //}
        //else
        //    shaderVariable = reinterpret_cast<Variable<T>*>(it->second.get());
        shaderVariable->Set(value);
    }

}}}}

#endif
