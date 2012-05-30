#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_VARIABLE_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_VARIABLE_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    class IVariable
    {
    public:
        virtual ~IVariable() {}

        virtual void UpdateParameter(int index) = 0;
    };

    template<class T>
    class Variable : public IVariable
    {
    private:
        T _value;
        std::vector<std::unique_ptr<IShaderParameter>> _parameters;

    public:
        Variable(std::vector<std::unique_ptr<IShaderParameter>>&& parameters)
        {
            this->_parameters.swap(parameters);
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
        Texture::ITexture* _texture;
        std::vector<std::unique_ptr<IShaderParameter>> _parameters;

    public:
        Variable(std::vector<std::unique_ptr<IShaderParameter>>&& parameters)
        {
            this->_parameters.swap(parameters);
        }

        void Set(Texture::ITexture& texture)
        {
            this->_texture = &texture;
        }

        virtual void UpdateParameter(int index)
        {
            if (this->_parameters[index] != 0)
                this->_parameters[index]->Set(this->_texture->GetCurrentTexture());
        }
    };

}}}}

#endif
