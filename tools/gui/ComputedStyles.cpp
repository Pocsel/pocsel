
#include "ComputedStyles.hpp"
#include "Widget.hpp"

using namespace Tools::Gui;

void ComputedStyles::Reload()
{
    this->_backgroundColor = Color4f(this->_widget.style("background-color"));
    this->_borderSize = Stylesheet::GetPixelSize(this->_widget.style("border-size"));
    this->_borderColor = Color4f(this->_widget.style("border-color"));
    this->_padding = Stylesheet::GetPixelSize(this->_widget.style("padding"));
    this->_margin = Stylesheet::GetPixelSize(this->_widget.style("margin"));
}
