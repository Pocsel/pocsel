
#include "tools/gui/widgets/Text.hpp"

using namespace Tools::Gui::Widgets;

Text::Text(std::string const& text,
           std::string const& className,
           std::string const& id) :
    Tools::Gui::Widget("text", className, id),
    _text(text)
{
}
void Text::Render(Tools::IRenderer& r)
{
    r.BeginDraw2D();
    r.SetModelMatrix(
        Matrix4<float>::CreateTranslation(this->geometry().pos.x, this->geometry().pos.y, 0)
    );
    this->font().Render(r, Color4f(0, 0, 0, 1), this->_text);
    r.EndDraw2D();
}
