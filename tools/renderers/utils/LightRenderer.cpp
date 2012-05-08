#include "tools/precompiled.hpp"

#include "tools/renderers/utils/DirectionnalLight.hpp"
#include "tools/renderers/utils/ILight.hpp"
#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/renderers/utils/LightRenderer.hpp"
#include "tools/renderers/utils/PointLight.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    LightRenderer::LightRenderer(IRenderer& renderer, IShaderProgram& directionnalShader, IShaderProgram& pointShader) :
        _renderer(renderer)
    {
        this->_directionnal.shader = &directionnalShader;
        this->_directionnal.normalDepth = this->_directionnal.shader->GetParameter("normalDepth");
        this->_directionnal.direction = this->_directionnal.shader->GetParameter("lightDirection");
        this->_directionnal.ambientColor = this->_directionnal.shader->GetParameter("lightAmbientColor");
        this->_directionnal.diffuseColor = this->_directionnal.shader->GetParameter("lightDiffuseColor");
        this->_directionnal.specularColor = this->_directionnal.shader->GetParameter("lightSpecularColor");
        this->_directionnal.screenModelViewProjection = this->_directionnal.shader->GetParameter("screenWorldViewProjection");
        this->_directionnal.screen.reset(new Image(renderer));
        this->_directionnal.modelViewProjection = glm::ortho(-0.5f, 0.5f, 0.5f, -0.5f) * glm::translate(0.0f, 0.0f, 1.0f);

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
        std::list<DirectionnalLight*> const& directionnalLights,
        std::list<PointLight*> const& pointLights)
    {
        gbuffer.BeginLighting();
        this->_renderer.SetClearColor(Color4f(.0f, .0f, .0f, 1.0f));
        this->_renderer.Clear(ClearFlags::Color);
        this->_renderer.SetDepthTest(false);
        this->_RenderDirectionnalLights(gbuffer, directionnalLights);
        this->_RenderPointLights(gbuffer, pointLights);
        gbuffer.EndLighting();
    }

    void LightRenderer::Render(
        GBuffer& gbuffer,
        std::list<DirectionnalLight> const& directionnalLights,
        std::list<PointLight> const& pointLights)
    {
        gbuffer.BeginLighting();
        this->_renderer.SetClearColor(Color4f(.0f, .0f, .0f, 1.0f));
        this->_renderer.Clear(ClearFlags::Color);
        this->_renderer.SetDepthTest(false);
        this->_RenderDirectionnalLights(gbuffer, directionnalLights);
        this->_RenderPointLights(gbuffer, pointLights);
        gbuffer.EndLighting();
    }

    void LightRenderer::_RenderDirectionnalLights(GBuffer& gbuffer, std::list<DirectionnalLight*> const& lights)
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
                (*it)->SetParameters();
                this->_directionnal.screen->Render(*this->_directionnal.normalDepth, gbuffer.GetNormalsDepth());
            }
        } while (this->_directionnal.shader->EndPass());
    }

    void LightRenderer::_RenderPointLights(GBuffer& gbuffer, std::list<PointLight*> const& lights)
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
                auto& light = **it;
                this->_renderer.SetModelMatrix(glm::scale(glm::vec3(light.range)) * glm::translate(light.position));
                light.SetParameters();
                this->_point.sphere->Render();
            }
        } while (this->_point.shader->EndPass());
        gbuffer.GetNormalsDepth().Unbind();
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
        this->_renderer.SetCullFace(false);
        gbuffer.GetNormalsDepth().Bind();
        this->_point.normalDepth->Set(gbuffer.GetNormalsDepth());
        do
        {
            this->_point.shader->BeginPass();
            for (auto it = lights.begin(), ite = lights.end(); it != ite; ++it)
            {
                auto& light = *it;
                this->_renderer.SetModelMatrix(glm::scale(glm::vec3(light.range)) * glm::translate(light.position));
                light.SetParameters();
                this->_point.sphere->Render();
            }
        } while (this->_point.shader->EndPass());
        gbuffer.GetNormalsDepth().Unbind();
        this->_renderer.SetCullFace(true);
    }

}}}
