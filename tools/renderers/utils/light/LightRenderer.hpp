#ifndef __TOOLS_RENDERERS_UTILS_LIGHTRENDERER_HPP__
#define __TOOLS_RENDERERS_UTILS_LIGHTRENDERER_HPP__

#include "tools/IRenderer.hpp"
#include "tools/Frustum.hpp"
#include "tools/renderers/utils/Image.hpp"
#include "tools/renderers/utils/Sphere.hpp"
#include "tools/renderers/utils/light/DirectionnalLight.hpp"
#include "tools/renderers/utils/light/PointLight.hpp"

namespace Tools { namespace Renderers { namespace Utils {
    class GBuffer;
    namespace Light {
        class ILight;
        class PointLight;
    }
}}}

namespace Tools { namespace Renderers { namespace Utils { namespace Light {

    class LightRenderer
    {
    private:
        IRenderer& _renderer;
        struct
        {
            IProgram* shader;
            IShaderParameter* normalDepth;
            IShaderParameter* ambientColor;
            IShaderParameter* direction;
            IShaderParameter* diffuseColor;
            IShaderParameter* specularColor;
            IShaderParameter* shadowMap;
            IShaderParameter* lightViewProjection;

            std::vector<std::pair<glm::mat4, std::unique_ptr<IRenderTarget>>> shadowMaps;

            glm::mat4 modelViewProjection;
            IShaderParameter* screenModelViewProjection;
            std::unique_ptr<Image> screen;
        } _directionnal;
        struct
        {
            IProgram* shader;
            IShaderParameter* normalDepth;
            IShaderParameter* position;
            IShaderParameter* range;
            IShaderParameter* diffuseColor;
            IShaderParameter* specularColor;
            IShaderParameter* shadowMap;

            std::vector<std::unique_ptr<IRenderTarget>> shadowMaps;

            std::unique_ptr<Sphere> sphere;
        } _point;

    public:
        LightRenderer(IRenderer& renderer, IProgram& directionnalShader, IProgram& pointShader);

        DirectionnalLight CreateDirectionnalLight();
        PointLight CreatePointLight();
        void Render(
            GBuffer& gbuffer,
            glm::mat4 const& view,
            glm::mat4 const& projection,
            Frustum const& absoluteCamera,
            glm::dvec3 const& position,
            std::function<void(glm::dmat4)>& renderScene,
            std::list<DirectionnalLight> const& directionnalLights,
            std::list<PointLight> const& pointLights);
        void Render(
            GBuffer& gbuffer,
            glm::mat4 const& view,
            glm::mat4 const& projection,
            Frustum const& absoluteCamera,
            glm::dvec3 const& position,
            std::function<void(glm::dmat4)>& renderScene,
            std::list<DirectionnalLight*> const& directionnalLights,
            std::list<PointLight*> const& pointLights);

        ITexture2D& GetDirectionnalShadowMap(int idx) { return this->_directionnal.shadowMaps[idx].second->GetTexture(0); }
    private:
        void _RenderDirectionnalLightsShadowMap(Frustum const& absoluteCamera, glm::dvec3 const& position, std::function<void(glm::dmat4)>& renderScene, std::list<DirectionnalLight> const& lights);
        template<class T>
        void _RenderDirectionnalLights(GBuffer& gbuffer, T& lights);
        template<class T>
        void _RenderPointLights(GBuffer& gbuffer, T& lights);
        template<class TDirectionnal, class TPoint>
        void _Render(
            GBuffer& gbuffer,
            glm::mat4 const& view,
            glm::mat4 const& projection,
            Frustum const& absoluteCamera,
            glm::dvec3 const& position,
            std::function<void(glm::dmat4)>& renderScene,
            TDirectionnal directionnalIterator,
            TPoint pointIterator);
    };

}}}}

#endif
