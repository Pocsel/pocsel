#include "tools/precompiled.hpp"

#include "tools/Vector3.hpp"
#include "tools/renderers/utils/DirectionnalLight.hpp"
#include "tools/renderers/utils/ILight.hpp"
#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/renderers/utils/LightRenderer.hpp"
#include "tools/renderers/utils/PointLight.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    LightRenderer::LightRenderer(IRenderer& renderer, IShaderProgram& depthShader, IShaderProgram& directionnalShader, IShaderProgram& pointShader) :
        _renderer(renderer)
    {
        int nbDirectionnalShadowMap = 4;

        this->_directionnal.shader = &directionnalShader;
        this->_directionnal.normalDepth = this->_directionnal.shader->GetParameter("normalDepth");
        this->_directionnal.direction = this->_directionnal.shader->GetParameter("lightDirection");
        this->_directionnal.ambientColor = this->_directionnal.shader->GetParameter("lightAmbientColor");
        this->_directionnal.diffuseColor = this->_directionnal.shader->GetParameter("lightDiffuseColor");
        this->_directionnal.specularColor = this->_directionnal.shader->GetParameter("lightSpecularColor");
        this->_directionnal.screenModelViewProjection = this->_directionnal.shader->GetParameter("screenWorldViewProjection");
        this->_directionnal.screen.reset(new Image(renderer));
        this->_directionnal.modelViewProjection = glm::ortho(-0.5f, 0.5f, 0.5f, -0.5f) * glm::translate(0.0f, 0.0f, 1.0f);
        for (int i = 0; i < nbDirectionnalShadowMap; ++i)
        {
            auto ptr = renderer.CreateRenderTarget(glm::uvec2(512, 512));
            ptr->PushRenderTarget(PixelFormat::R32f, RenderTargetUsage::Color);
            ptr->PushRenderTarget(PixelFormat::Depth24Stencil8, RenderTargetUsage::DepthStencil);
            this->_directionnal.shadowMaps.push_back(std::move(ptr));
        }

        this->_point.shader = &pointShader;
        this->_point.normalDepth = this->_point.shader->GetParameter("normalDepth");
        this->_point.position = this->_point.shader->GetParameter("lightPosition");
        this->_point.range = this->_point.shader->GetParameter("lightRange");
        this->_point.diffuseColor = this->_point.shader->GetParameter("lightDiffuseColor");
        this->_point.specularColor = this->_point.shader->GetParameter("lightSpecularColor");
        this->_point.sphere.reset(new Sphere(renderer));
    }

    DirectionnalLight LightRenderer::CreateDirectionnalLight()
    {
        return DirectionnalLight(
            *this->_directionnal.direction,
            *this->_directionnal.ambientColor,
            *this->_directionnal.diffuseColor,
            *this->_directionnal.specularColor);
    }

    PointLight LightRenderer::CreatePointLight()
    {
        return PointLight(
            *this->_point.position,
            *this->_point.range,
            *this->_directionnal.diffuseColor,
            *this->_directionnal.specularColor);
    }

    void LightRenderer::Render(
        GBuffer& gbuffer,
        Frustum const& camera,
        std::function<void(glm::dmat4)>& renderScene,
        std::list<DirectionnalLight> const& directionnalLights,
        std::list<PointLight> const& pointLights)
    {
        this->_RenderDirectionnalLightsShadowMap(camera, renderScene, directionnalLights);

        gbuffer.BeginLighting();
        this->_renderer.SetClearColor(Color4f(.0f, .0f, .0f, 1.0f));
        this->_renderer.Clear(ClearFlags::Color);
        this->_renderer.SetDepthTest(false);
        this->_RenderPointLights(gbuffer, pointLights);
        this->_RenderDirectionnalLights(gbuffer, directionnalLights);
        gbuffer.EndLighting();
    }

    void LightRenderer::_RenderDirectionnalLights(GBuffer& gbuffer, std::list<DirectionnalLight> const& lights)
    {
        if (lights.size() == 0)
            return;
        this->_renderer.SetModelMatrix(glm::mat4::identity);
        //this->_directionnal.normalDepth->Set(gbuffer.GetSpecular());
        this->_directionnal.screenModelViewProjection->Set(this->_directionnal.modelViewProjection, true);
        do
        {
            this->_directionnal.shader->BeginPass();
            for (auto it = lights.begin(), ite = lights.end(); it != ite; ++it)
            {
                it->SetParameters();
                this->_directionnal.screen->Render(*this->_directionnal.normalDepth, gbuffer.GetNormalsDepth());
            }
        } while (this->_directionnal.shader->EndPass());
    }

    void LightRenderer::_RenderPointLights(GBuffer& gbuffer, std::list<PointLight> const& lights)
    {
        if (lights.size() == 0)
            return;
        gbuffer.GetNormalsDepth().Bind();
        this->_point.normalDepth->Set(gbuffer.GetNormalsDepth());
        do
        {
            this->_point.shader->BeginPass();
            for (auto it = lights.begin(), ite = lights.end(); it != ite; ++it)
            {
                auto& light = *it;
                this->_renderer.SetModelMatrix(glm::scale(glm::translate(light.position), glm::vec3(light.range)));
                light.SetParameters();
                this->_point.sphere->Render();
            }
        } while (this->_point.shader->EndPass());
        gbuffer.GetNormalsDepth().Unbind();
    }

    void LightRenderer::_RenderDirectionnalLightsShadowMap(Frustum const& camera, std::function<void(glm::dmat4)>& renderScene, std::list<DirectionnalLight> const& lights)
    {
        glm::dvec3 const cameraCorners[8] = {
            camera.GetCorners()[0], camera.GetCorners()[1],
            camera.GetCorners()[2], camera.GetCorners()[3],
            camera.GetCorners()[4], camera.GetCorners()[5],
            camera.GetCorners()[6], camera.GetCorners()[7],
        };

        int i = 0;
        for (auto it = lights.begin(), ite = lights.end(); it != ite && i < this->_directionnal.shadowMaps.size(); ++it, ++i)
        {
            // On calcule la matrix light ViewProjection
            auto lightPos = glm::dvec3(camera.GetCenter()) - camera.GetRadius() * glm::dvec3(it->direction);
            auto lightView = glm::lookAt(lightPos, lightPos + glm::dvec3(it->direction), glm::dvec3(0.0, 1.0, 0.0));
            auto tmp = glm::dmat3(lightView);
            glm::dvec3 lightCorners[8] = {
                tmp * cameraCorners[0], tmp * cameraCorners[1],
                tmp * cameraCorners[2], tmp * cameraCorners[3],
                tmp * cameraCorners[4], tmp * cameraCorners[5],
                tmp * cameraCorners[6], tmp * cameraCorners[7],
            };
            glm::dvec3 min = lightCorners[0];
            glm::dvec3 max = lightCorners[0];
            for (int j = 1; j < 8; ++j)
            {
                min.x = std::min(min.x, lightCorners[j].x);
                min.y = std::min(min.y, lightCorners[j].y);
                min.z = std::min(min.z, lightCorners[j].z);
                max.x = std::max(max.x, lightCorners[j].x);
                max.y = std::max(max.y, lightCorners[j].y);
                max.z = std::max(max.z, lightCorners[j].z);
            }
            auto lightViewProjection = glm::ortho(min.x, max.x, min.z, max.z, min.y, max.y) * lightView;

            this->_renderer.BeginDraw(this->_directionnal.shadowMaps[i].get());
            renderScene(lightViewProjection);
            this->_renderer.EndDraw();
        }
    }

}}}
