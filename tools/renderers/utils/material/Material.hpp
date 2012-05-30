#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_MATERIAL_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_MATERIAL_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/material/Variable.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    class Material
    {
    private:
        struct Effect
        {
            IShaderProgram& shader;
            std::vector<std::pair<std::unique_ptr<IShaderParameter>, float>> scalars;
            std::vector<std::pair<std::unique_ptr<IShaderParameter>, glm::vec2>> vectors2;
            std::vector<std::pair<std::unique_ptr<IShaderParameter>, glm::vec3>> vectors3;
            std::vector<std::pair<std::unique_ptr<IShaderParameter>, glm::vec4>> vectors4;
            std::vector<std::pair<std::unique_ptr<IShaderParameter>, Texture::ITexture*>> textures;
            std::unique_ptr<IShaderParameter> totalTime;

            Effect(IShaderProgram& shader) : shader(shader) {}
            void UpdateParameters(Uint64 totalTime);
        };

        IRenderer& _renderer;
        Effect _geometry;
        Effect _shadowMap;
        std::vector<std::unique_ptr<Texture::ITexture>> _textures;
        std::vector<std::unique_ptr<IVariable>> _variables;

    public:
        Material(IRenderer& renderer, IShaderProgram& geometry, IShaderProgram& shadowMap);

        void AddConstant(std::string const& name, float value);
        void AddConstant(std::string const& name, glm::vec2 const& value);
        void AddConstant(std::string const& name, glm::vec3 const& value);
        void AddConstant(std::string const& name, glm::vec4 const& value);
        void AddTexture(std::string const& name, std::unique_ptr<Texture::ITexture>&& texture);
        template<class TValue>
        Variable<TValue>& GetVariable(std::string const& name);
        void SetTimeParameter(std::string const& name);

        void Update(Uint64 totalTime); // totalTime in microsecond
        void RenderGeometry(std::function<void()>& render, Uint64 totalTime);
        void RenderShadowMap(std::function<void()>& render, Uint64 totalTime);
    };

    template<class TValue>
    inline Variable<TValue>& Material::GetVariable(std::string const& name)
    {
        std::vector<std::unique_ptr<IShaderParameter>> parameters;
        parameters.push_back(this->_geometry.shader.GetParameter(name));
        parameters.push_back(this->_shadowMap.shader.GetParameter(name));
        auto ptr = new Variable<TValue>(std::move(parameters));
        this->_variables.push_back(std::unique_ptr<IVariable>(ptr));
        return *ptr;
    }

}}}}

#endif
