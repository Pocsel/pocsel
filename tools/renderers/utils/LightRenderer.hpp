#ifndef __TOOLS_RENDERERS_UTILS_LIGHTRENDERER_HPP__
#define __TOOLS_RENDERERS_UTILS_LIGHTRENDERER_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/DirectionnalLight.hpp"
#include "tools/renderers/utils/Rectangle.hpp"

namespace Tools { namespace Renderers { namespace Utils {
    class GBuffer;
    class ILight;
    class PointLight;
}}}

namespace Tools { namespace Renderers { namespace Utils {

    class LightRenderer
    {
    private:
        IRenderer& _renderer;
        struct
        {
            std::unique_ptr<IShaderProgram> shader;
            std::unique_ptr<IShaderParameter> direction;
            std::unique_ptr<IShaderParameter> diffuseColor;
            std::unique_ptr<IShaderParameter> specularColor;
            std::unique_ptr<IShaderParameter> specularPower;
            glm::mat4 modelViewProjection;
            std::unique_ptr<IShaderParameter> screenModelViewProjection;
            std::unique_ptr<Rectangle> screen;
        } _directionnal;
        struct
        {
            std::unique_ptr<IShaderProgram> shader;
            std::unique_ptr<IShaderParameter> position;
            std::unique_ptr<IShaderParameter> radius;
            std::unique_ptr<IShaderParameter> diffuseColor;
            std::unique_ptr<IShaderParameter> specularColor;
            std::unique_ptr<IShaderParameter> specularPower;
        } _point;

    public:
        LightRenderer(IRenderer& renderer, std::string const& directionnalShader, std::string const& pointShader);

        DirectionnalLight CreateDirectionnalLight();
        //PointLight CreatePointLight();
        void Render(
            GBuffer& gbuffer,
            std::list<DirectionnalLight*> const& directionnalLights,
            std::list<PointLight*> const& pointLights);
    private:
        void _RenderDirectionnalLights(std::list<DirectionnalLight*> const& lights);
        void _RenderPointLights(std::list<PointLight*> const& lights);
    };

}}}

#endif
