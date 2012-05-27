#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Tools { namespace Renderers { namespace Utils {

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

    public:
        Material(IRenderer& renderer, IShaderProgram& geometry, IShaderProgram& shadowMap);

        void AddConstant(std::string const& name, float value);
        void AddConstant(std::string const& name, glm::vec2 const& value);
        void AddConstant(std::string const& name, glm::vec3 const& value);
        void AddConstant(std::string const& name, glm::vec4 const& value);
        void AddTexture(std::string const& name, std::unique_ptr<Texture::ITexture>&& texture);
        void SetTimeParameter(std::string const& name);

        void Update(Uint64 totalTime); // totalTime in microsecond
        void RenderGeometry(std::function<void()>& render, Uint64 totalTime);
        void RenderShadowMap(std::function<void()>& render, Uint64 totalTime);
    };

}}}

#endif
