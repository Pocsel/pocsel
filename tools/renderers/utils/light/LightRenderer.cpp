#include "tools/precompiled.hpp"

#include "tools/Vector3.hpp"
#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/renderers/utils/light/DirectionnalLight.hpp"
#include "tools/renderers/utils/light/ILight.hpp"
#include "tools/renderers/utils/light/LightRenderer.hpp"
#include "tools/renderers/utils/light/PointLight.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Light {

    namespace {
        void _CalculateLightViewAndProjection(Frustum const& camera, glm::dvec3 const& cameraPosition, glm::dvec3 const& direction, glm::dmat4& view, glm::dmat4 projection)
        {
            //glm::dvec3 const absoluteCameraCorners[8] = {
            //    camera.GetCorners()[0] - cameraPosition, camera.GetCorners()[1] - cameraPosition,
            //    camera.GetCorners()[2] - cameraPosition, camera.GetCorners()[3] - cameraPosition,
            //    camera.GetCorners()[4] - cameraPosition, camera.GetCorners()[5] - cameraPosition,
            //    camera.GetCorners()[6] - cameraPosition, camera.GetCorners()[7] - cameraPosition,
            //};

            //// On calcule les matrices light ViewProjection
            //auto position = glm::dvec3(camera.GetCenter()) - camera.GetRadius() * direction;
            //view = glm::lookAt(position, position + direction, glm::dvec3(0.0, 1.0, 0.0));
            //auto tmp = glm::dmat3(view);
            //glm::dvec3 lightCorners[8] = {
            //    tmp * absoluteCameraCorners[0], tmp * absoluteCameraCorners[1],
            //    tmp * absoluteCameraCorners[2], tmp * absoluteCameraCorners[3],
            //    tmp * absoluteCameraCorners[4], tmp * absoluteCameraCorners[5],
            //    tmp * absoluteCameraCorners[6], tmp * absoluteCameraCorners[7],
            //};
            //glm::dvec3 min = lightCorners[0];
            //glm::dvec3 max = lightCorners[0];
            //for (int j = 1; j < 8; ++j)
            //{
            //    min.x = std::min(min.x, lightCorners[j].x);
            //    min.y = std::min(min.y, lightCorners[j].y);
            //    min.z = std::min(min.z, lightCorners[j].z);
            //    max.x = std::max(max.x, lightCorners[j].x);
            //    max.y = std::max(max.y, lightCorners[j].y);
            //    max.z = std::max(max.z, lightCorners[j].z);
            //}
            //projection = glm::ortho(min.x, max.x, min.z, max.z, min.y, max.y);

            auto lightRotation = glm::lookAt(glm::dvec3(0.0, 0.0, 0.0), -direction, glm::dvec3(0.0, 1.0, 0.0));
            glm::dvec3 frustumCorners[8] = {
                camera.GetCorners()[0] - cameraPosition, camera.GetCorners()[1] - cameraPosition,
                camera.GetCorners()[2] - cameraPosition, camera.GetCorners()[3] - cameraPosition,
                camera.GetCorners()[4] - cameraPosition, camera.GetCorners()[5] - cameraPosition,
                camera.GetCorners()[6] - cameraPosition, camera.GetCorners()[7] - cameraPosition,
            };

            for (int i = 0; i < 8; ++i)
                frustumCorners[i] = glm::dvec3(lightRotation * glm::dvec4(frustumCorners[i], 1.0));
            glm::dvec3 min = frustumCorners[0];
            glm::dvec3 max = frustumCorners[0];
            for (int j = 1; j < 8; ++j)
            {
                min.x = std::min(min.x, frustumCorners[j].x);
                min.y = std::min(min.y, frustumCorners[j].y);
                min.z = std::min(min.z, frustumCorners[j].z);
                max.x = std::max(max.x, frustumCorners[j].x);
                max.y = std::max(max.y, frustumCorners[j].y);
                max.z = std::max(max.z, frustumCorners[j].z);
            }
            glm::dvec3 size = max - min;
            glm::dvec3 halfSize = size * 0.5;
            glm::dvec3 lightPosition = min + halfSize;
            lightPosition.z = min.z;

            lightPosition = glm::dvec3(glm::inverse(lightRotation) * glm::dvec4(lightPosition, 1.0));

            view = glm::lookAt(lightPosition, lightPosition - direction, glm::dvec3(0.0, 1.0, 0.0));
            projection = glm::ortho(size.x, size.y, -size.z, size.z);

        }
    }

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
        this->_directionnal.shadowMap = this->_directionnal.shader->GetParameter("lightShadowMap");
        this->_directionnal.lightViewProjection = this->_directionnal.shader->GetParameter("lightViewProjection");

        this->_directionnal.depthShader = &depthShader;
        this->_directionnal.screen.reset(new Image(renderer));
        this->_directionnal.modelViewProjection = glm::ortho(-0.5f, 0.5f, 0.5f, -0.5f) * glm::translate(0.0f, 0.0f, 1.0f);
        for (int i = 0; i < nbDirectionnalShadowMap; ++i)
        {
            auto ptr = renderer.CreateRenderTarget(glm::uvec2(512, 512));
            ptr->PushRenderTarget(PixelFormat::R32f, RenderTargetUsage::Color);
            ptr->PushRenderTarget(PixelFormat::Depth24Stencil8, RenderTargetUsage::DepthStencil);
            this->_directionnal.shadowMaps.push_back(std::make_pair(glm::mat4(), std::move(ptr)));
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
        glm::mat4 const& view,
        glm::mat4 const& projection,
        Frustum const& absoluteCamera,
        glm::dvec3 const& position,
        std::function<void(glm::dmat4)>& renderScene,
        std::list<DirectionnalLight> const& directionnalLights,
        std::list<PointLight> const& pointLights)
    {
        this->_RenderDirectionnalLightsShadowMap(absoluteCamera, position, renderScene, directionnalLights);

        gbuffer.BeginLighting();

        this->_renderer.SetViewMatrix(view);
        this->_renderer.SetProjectionMatrix(projection);

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
            int i = 0;
            for (auto it = lights.begin(), ite = lights.end(); it != ite; ++it)
            {
                it->SetParameters();
                if (i < this->_directionnal.shadowMaps.size())
                {
                    this->_directionnal.lightViewProjection->Set(this->_directionnal.shadowMaps[i].first);
                    this->_directionnal.shadowMap->Set(this->_directionnal.shadowMaps[i].second->GetTexture(0));
                }
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

    void LightRenderer::_RenderDirectionnalLightsShadowMap(Frustum const& absoluteCamera, glm::dvec3 const& cameraPosition,  std::function<void(glm::dmat4)>& renderScene, std::list<DirectionnalLight> const& lights)
    {
        int i = 0;
        for (auto it = lights.begin(), ite = lights.end(); it != ite && i < this->_directionnal.shadowMaps.size(); ++it, ++i)
        {
            glm::dmat4 lightView;
            glm::dmat4 lightProjection;
            _CalculateLightViewAndProjection(absoluteCamera, glm::dvec3(0, 0, 0), glm::dvec3(it->direction), lightView, lightProjection);
            auto lightAbsoluteViewProjection = lightProjection * lightView;
            _CalculateLightViewAndProjection(absoluteCamera, cameraPosition, glm::dvec3(it->direction), lightView, lightProjection);

            this->_renderer.SetViewMatrix(glm::mat4(lightView));
            this->_renderer.SetProjectionMatrix(glm::mat4(lightProjection));

            this->_directionnal.shadowMaps[i].first = glm::mat4(lightAbsoluteViewProjection);
            this->_renderer.BeginDraw(this->_directionnal.shadowMaps[i].second.get());
            do
            {
                this->_directionnal.depthShader->BeginPass();
                renderScene(lightAbsoluteViewProjection);
            } while (this->_directionnal.depthShader->EndPass());
            this->_renderer.EndDraw();
        }
    }

}}}}
