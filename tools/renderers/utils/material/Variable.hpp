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
        std::vector<std::unique_ptr<IShaderParameter>> _parameters;

    public:
        Variable(Material&, std::vector<std::unique_ptr<IShaderParameter>>&& parameters)
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
        std::vector<std::unique_ptr<IShaderParameter>> _parameters;

    public:
        Variable(Material& material, std::vector<std::unique_ptr<IShaderParameter>>&& parameters) :
            _material(material),
            _texture(0)
        {
            this->_parameters.swap(parameters);
        }

        Texture::ITexture& Get()
        {
            return *this->_texture;
        }

        void Set(std::unique_ptr<Texture::ITexture>&& texture)
        {
            this->_texture = texture.get();
            this->_material.SetTextures(*this, std::move(texture));
        }

        virtual void UpdateParameter(int index)
        {
            if (this->_texture != 0 && this->_parameters[index] != 0)
                this->_parameters[index]->Set(this->_texture->GetCurrentTexture());
        }
    };

    template<class TValue>
    inline Variable<TValue>& Material::GetVariable(std::string const& name)
    {
        std::vector<std::unique_ptr<IShaderParameter>> parameters;
        parameters.push_back(this->_geometry.shader.GetParameter(name));
        parameters.push_back(this->_shadowMap.shader.GetParameter(name));
        auto ptr = new Variable<TValue>(*this, std::move(parameters));
        this->_variables.push_back(std::unique_ptr<IVariable>(ptr));
        return *ptr;
    }

}}}}

#endif
