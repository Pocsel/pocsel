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
        this->_point.shader = renderer.CreateProgram(pointShader);
    }

    DirectionnalLight LightRenderer::CreateDirectionnalLight()
    {
        return DirectionnalLight(
            *this->_directionnal.direction,
            *this->_directionnal.diffuseColor,
            *this->_directionnal.diffusePower,
            *this->_directionnal.specularColor,
            *this->_directionnal.specularPower);
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
            for (auto it = lights.begin(), ite = lights.end(); it != ite; ++it)
            {
                (*it)->SetParameters();
                this->_directionnal.screen.Render();
            }
        } while (this->_directionnal.shader->EndPass());
    }

    void LightRenderer::_RenderPointLights(std::list<PointLight*> const& lights)
    {
        // TODO
    }

}}}
