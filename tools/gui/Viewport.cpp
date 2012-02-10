#include "tools/gui/Viewport.hpp"

#include "tools/gui/events/all.hpp"

namespace Tools { namespace Gui {

    Viewport::Viewport(std::string const& className,
                       std::string const& id) :
        Container("viewport", className, id)
    {
        this->_backgroundColor = Color3f(this->style("background-color"));
    }

#undef Expose
    void Viewport::OnExpose(Events::Expose const& evt)
    {
        this->_Resize(evt.width, evt.height);
    }

    void Viewport::OnResize(Events::Resize const& evt)
    {
        this->_Resize(evt.width, evt.height);
        auto it = this->_childs.begin(),
             end = this->_childs.end();
        for (; it != end; ++it)
            (*it)->OnResize(evt);
    }

    void Viewport::Render(Tools::IRenderer& r)
    {
        if (this->size().w == 0 || this->size().h == 0)
            return;

        r.SetViewport(this->geometry());
        auto it = this->_childs.begin(),
             end = this->_childs.end();
        for (; it != end; ++it)
            (*it)->Render(r);
    }

    void Viewport::_Resize(size_t w, size_t h)
    {
        this->size(w, h);
    }

}}
