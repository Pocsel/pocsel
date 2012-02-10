#ifndef __TOOLS_GUI_COMPUTEDSTYLES_HPP__
#define __TOOLS_GUI_COMPUTEDSTYLES_HPP__

#include "tools/Color.hpp"

namespace Tools { namespace Gui {

    class Widget;
    class Stylesheet;

    class ComputedStyles : private boost::noncopyable
    {
    private:
        Widget&             _widget;
        Color4f             _backgroundColor;
        size_t              _borderSize;
        Color4f             _borderColor;
        size_t              _padding;
        size_t              _margin;

    public:
        ComputedStyles(Widget& widget) :
            _widget(widget)
        {
            this->Reload();
        }
        void Reload();

    public: // getters
        Color4f const& GetBackgroundColor() const { return this->_backgroundColor; }
        size_t GetBorderSize() const { return this->_borderSize; }
        Color4f const& GetBorderColor() const { return this->_borderColor; }
        size_t GetPadding() const { return this->_padding; }
        size_t GetMargin() const { return this->_margin; }
    };

}}

#endif
