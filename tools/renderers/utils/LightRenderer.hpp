#ifndef __TOOLS_RENDERERS_UTILS_LIGHTRENDERER_HPP__
#define __TOOLS_RENDERERS_UTILS_LIGHTRENDERER_HPP__

#include "tools/IRenderer.hpp"
#include "tools/Frustum.hpp"
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
            std::unique_ptr<IShaderParameter> shadowMap;
            std::unique_ptr<IShaderParameter> lightViewProjection;

            IShaderProgram* depthShader;
            std::vector<std::pair<glm::mat4, std::unique_ptr<IRenderTarget>>> shadowMaps;

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
            std::unique_ptr<IShaderParameter> shadowMap;

            IShaderProgram* depthShader;
            std::vector<std::unique_ptr<IRenderTarget>> shadowMaps;

            std::unique_ptr<Sphere> sphere;
        } _point;

    public:
        LightRenderer(IRenderer& renderer, IShaderProgram& depthShader, IShaderProgram& directionnalShader, IShaderProgram& pointShader);

        DirectionnalLight CreateDirectionnalLight();
        PointLight CreatePointLight();
        void Render(
            GBuffer& gbuffer,
            Frustum const& absoluteCamera,
            glm::dvec3 const& position,
            std::function<void(glm::dmat4)>& renderScene,
            std::list<DirectionnalLight> const& directionnalLights,
            std::list<PointLight> const& pointLights);

        ITexture2D& GetDirectionnalShadowMap(int idx) { return this->_directionnal.shadowMaps[idx].second->GetTexture(0); }
    private:
        void _RenderDirectionnalLights(GBuffer& gbuffer, std::list<DirectionnalLight> const& lights);
        void _RenderPointLights(GBuffer& gbuffer, std::list<PointLight> const& lights);
        void _RenderDirectionnalLightsShadowMap(Frustum const& absoluteCamera, glm::dvec3 const& position, std::function<void(glm::dmat4)>& renderScene, std::list<DirectionnalLight> const& lights);
    };

}}}

#endif
