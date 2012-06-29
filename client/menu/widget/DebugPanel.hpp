#ifndef __CLIENT_MENU_WIDGET_DEBUGPANEL_HPP__
#define __CLIENT_MENU_WIDGET_DEBUGPANEL_HPP__

#include "tools/renderers/utils/Rectangle.hpp"
#include "tools/Vector2.hpp"
#include "tools/Matrix4.hpp"

namespace Tools {
    namespace Stat {
        class IStat;
    }
    class IRenderer;
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
        Tools::IRenderer& _renderer;
        Tools::Renderers::Utils::Rectangle _rect;
        std::unordered_map<Tools::Stat::IStat*, std::list<double>> _averages;
        std::unordered_map<Tools::Stat::IStat*, std::pair<double, double>> _minMax;
        Uint32 _maxAverageValues;

    public:
        DebugPanel(Menu& menu, Tools::IRenderer& renderer, Uint32 maxAverageValues = 120);
        void Render();
    private:
        void _Update();
    };

}}}

#endif
