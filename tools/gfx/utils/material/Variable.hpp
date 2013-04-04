#pragma once

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/utils/material/Material.hpp"
#include "tools/gfx/utils/texture/ITexture.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Material {

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

        virtual Luasel::Ref GetLuaValue(Luasel::Interpreter& interpreter) const
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

        virtual Luasel::Ref GetLuaValue(Luasel::Interpreter& interpreter) const
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
            param = (this->_shadowMap != 0) ? &this->_shadowMap->shader.GetParameter(name) : 0;
            parameters.push_back(param != 0 && param->IsUseable() ? param : 0);
            auto ptr = new Variable<TValue>(*this, std::move(parameters));
            this->_variables.insert(std::move(std::make_pair(name, std::unique_ptr<IVariable>(ptr))));
            return *ptr;
        }
        return reinterpret_cast<Variable<TValue>&>(*it->second);
    }

    template<class T>
    inline void Material::_SetValue(std::string const& key, T value)
    {
        this->GetVariable<T>(key).Set(value);
    }

    template<class T>
    inline void Material::_SetValue(std::string const& key, std::shared_ptr<T> value)
    {
        this->GetVariable<T>(key).Set(value);
    }

}}}}
