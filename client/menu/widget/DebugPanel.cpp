#include "client/precompiled.hpp"

#include "tools/gfx/effect/Effect.hpp"
#include "tools/gfx/utils/Rectangle.hpp"
#include "tools/gfx/utils/Font.hpp"
#include "tools/stat/StatManager.hpp"

#include "client/menu/widget/DebugPanel.hpp"
#include "client/menu/Menu.hpp"

namespace Client { namespace Menu { namespace Widget {

    DebugPanel::DebugPanel(Menu& menu, Tools::Gfx::IRenderer& renderer, Uint32 maxAverageValues) :
        _menu(menu),
        _renderer(renderer),
        _rect(renderer),
        _maxAverageValues(maxAverageValues)
    {
        this->_rect.SetColor(
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        this->_rect.SetPosition(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
    }


    void DebugPanel::Render()
    {
        float offset = 0.0f;
        auto const& stats = Tools::Stat::statManager.GetStats();
        for (auto it = stats.begin(), ite = stats.end(); it != ite; ++it)
        {
            auto const& name = it->first->GetName();
            auto const& value = it->first->GetValue();
            auto& averages = this->_averages[it->first];
            auto& minMax = this->_minMax[it->first];
            averages.push_back(value);
            minMax.first = std::min(minMax.first, value);
            minMax.second = std::max(minMax.second, value);
            while (averages.size() > this->_maxAverageValues)
                averages.pop_front();

            this->_renderer.SetModelMatrix(glm::translate(glm::vec3(0.0f, offset, 0.0f)) * glm::scale(0.5f, 0.5f, 1.0f));
            auto& ftShader = this->_menu.GetFontShader();
            do
            {
                ftShader.BeginPass();
                this->_menu.GetFont().Render(this->_menu.GetFontTexture(), name + ": " + Tools::ToString(value));
            } while (ftShader.EndPass());

            double delta = std::max(0.01, minMax.second - minMax.first);

            float offsetX = 250.0f;
            auto& rectShader = this->_menu.GetRectShader();
            for (auto avgIt = averages.begin(), avgIte = averages.end(); avgIt != avgIte; ++avgIt)
            {
                float sy = float((*avgIt - minMax.first) / delta) * 30.0f;
                this->_renderer.SetModelMatrix(glm::translate(offsetX, offset + 30.0f - sy, 0.0f) * glm::scale(3.0f, sy, 1.0f));
                offsetX += 3.0f;
                //this->_renderer.SetModelMatrix(glm::scale(0.5f, 0.5f, 1.0f));
                do
                {
                    rectShader.BeginPass();
                    this->_rect.Render();
                } while (rectShader.EndPass());
            }

            offset += 32.0f;
        }
    }

    void DebugPanel::_Update()
    {
    }
}}}
