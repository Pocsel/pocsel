#ifndef __CLIENT_MENU_WIDGET_DEBUGPANEL_HPP__
#define __CLIENT_MENU_WIDGET_DEBUGPANEL_HPP__

#include "tools/gfx/utils/Rectangle.hpp"
#include "tools/Vector2.hpp"

namespace Tools {
    namespace Stat {
        class IStat;
    }
    namespace Gfx {
        class IRenderer;
    }
}
namespace Client { namespace Menu {
    class Menu;
}}

namespace Client { namespace Menu { namespace Widget {

    class DebugPanel :
        private boost::noncopyable
    {
    private:
        Menu& _menu;
        Tools::Gfx::IRenderer& _renderer;
        Tools::Gfx::Utils::Rectangle _rect;
        std::unordered_map<Tools::Stat::IStat*, std::list<double>> _averages;
        std::unordered_map<Tools::Stat::IStat*, std::pair<double, double>> _minMax;
        Uint32 _maxAverageValues;

    public:
        DebugPanel(Menu& menu, Tools::Gfx::IRenderer& renderer, Uint32 maxAverageValues = 120);
        void Render();
    private:
        void _Update();
    };

}}}

#endif
