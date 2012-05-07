#include "tools/precompiled.hpp"

#include "tools/renderers/utils/DirectionnalLight.hpp"
#include "tools/renderers/utils/ILight.hpp"
#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/renderers/utils/LightRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    LightRenderer::LightRenderer(IRenderer& renderer, std::string const& directionnalShader, std::string const& pointShader) :
        _renderer(renderer)
    {
        this->_directionnal.shader = renderer.CreateProgram(directionnalShader);
        this->_directionnal.screen = std::unique_ptr<Rectangle>(new Rectangle(renderer));
        this->_directionnal.modelViewProjection = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f) * glm::translate(0.0f, 0.0f, 1.0f);
        this->_directionnal.direction = this->_directionnal.shader->GetParameter("direction");
        this->_directionnal.diffuseColor = this->_directionnal.shader->GetParameter("diffuseColor");
        this->_directionnal.specularColor = this->_directionnal.shader->GetParameter("specularColor");
        this->_directionnal.specularPower = this->_directionnal.shader->GetParameter("specularPower");
        this->_directionnal.screenModelViewProjection = this->_directionnal.shader->GetParameter("screenWorldViewProjection");

        this->_point.shader = renderer.CreateProgram(pointShader);
    }

    DirectionnalLight LightRenderer::CreateDirectionnalLight()
    {
        return DirectionnalLight(
            *this->_directionnal.direction,
            *this->_directionnal.diffuseColor,
            *this->_directionnal.specularColor);
    }

    //PointLight LightRenderer::CreatePointLight()
    //{
    //}

    void LightRenderer::Render(
        GBuffer& gbuffer,
        std::list<DirectionnalLight*> const& directionnalLights,
        std::list<PointLight*> const& pointLights)
    {
        gbuffer.BeginLighting();
        this->_RenderDirectionnalLights(directionnalLights);
        this->_RenderPointLights(pointLights);
        gbuffer.EndLighting();
    }

    void LightRenderer::_RenderDirectionnalLights(std::list<DirectionnalLight*> const& lights)
    {
        do
        {
            this->_directionnal.shader->BeginPass();
            this->_directionnal.screenModelViewProjection->Set(this->_directionnal.modelViewProjection, true);
            for (auto it = lights.begin(), ite = lights.end(); it != ite; ++it)
            {
                (*it)->SetParameters();
                this->_directionnal.screen->Render();
            }
        } while (this->_directionnal.shader->EndPass());
    }

    void LightRenderer::_RenderPointLights(std::list<PointLight*> const& lights)
    {
        // TODO
    }

}}}
