#ifndef __TOOLS_RENDERERS_UTILS_LIGHTRENDERER_HPP__
#define __TOOLS_RENDERERS_UTILS_LIGHTRENDERER_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/DirectionnalLight.hpp"
#include "tools/renderers/utils/Image.hpp"
#include "tools/renderers/utils/PointLight.hpp"
#include "tools/renderers/utils/Sphere.hpp"

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
            IShaderProgram* shader;
            std::unique_ptr<IShaderParameter> normalDepth;
            std::unique_ptr<IShaderParameter> ambientColor;
            std::unique_ptr<IShaderParameter> direction;
            std::unique_ptr<IShaderParameter> diffuseColor;
            std::unique_ptr<IShaderParameter> specularColor;
            glm::mat4 modelViewProjection;
            std::unique_ptr<IShaderParameter> screenModelViewProjection;
            std::unique_ptr<Image> screen;
        } _directionnal;
        struct
        {
            IShaderProgram* shader;
            std::unique_ptr<IShaderParameter> normalDepth;
            std::unique_ptr<IShaderParameter> position;
            std::unique_ptr<IShaderParameter> range;
            std::unique_ptr<IShaderParameter> diffuseColor;
            std::unique_ptr<IShaderParameter> specularColor;
            std::unique_ptr<Sphere> sphere;
        } _point;

    public:
        LightRenderer(IRenderer& renderer, IShaderProgram& directionnalShader, IShaderProgram& pointShader);

        DirectionnalLight CreateDirectionnalLight();
        PointLight CreatePointLight();
        void Render(
            GBuffer& gbuffer,
            std::list<DirectionnalLight*> const& directionnalLights,
            std::list<PointLight*> const& pointLights);
        void Render(
            GBuffer& gbuffer,
            std::list<DirectionnalLight> const& directionnalLights,
            std::list<PointLight> const& pointLights);
    private:
        void _RenderDirectionnalLights(GBuffer& gbuffer, std::list<DirectionnalLight*> const& lights);
        void _RenderPointLights(GBuffer& gbuffer, std::list<PointLight*> const& lights);
        void _RenderDirectionnalLights(GBuffer& gbuffer, std::list<DirectionnalLight> const& lights);
        void _RenderPointLights(GBuffer& gbuffer, std::list<PointLight> const& lights);
    };

}}}

#endif
