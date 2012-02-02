#ifndef __TOOLS_GUI_WIDGETS_TEXT_HPP__
#define __TOOLS_GUI_WIDGETS_TEXT_HPP__

#include "tools/IRenderer.hpp"
#include "tools/gui/Widget.hpp"

namespace Tools { namespace Gui { namespace Widgets {

    class Text : public Tools::Gui::Widget
    {
    private:
        std::string _text;

    public:
        Text(std::string const& text,
             std::string const& className = "",
             std::string const& id = "");
        virtual void Render(Tools::IRenderer& r);
        std::string const& text() { return this->_text; }
        void text(std::string const& text) { this->_text = text; }
    };

}}}

#endif
