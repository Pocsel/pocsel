#ifndef __TOOLS_GUI_VIEWPORT_HPP__
#define __TOOLS_GUI_VIEWPORT_HPP__

#include "tools/Color.hpp"
#include "tools/IRenderer.hpp"

#include "tools/gui/events/all_forward.hpp"

#include "tools/gui/Container.hpp"


namespace Tools { namespace Gui {

    class Viewport : public Container
    {
    private:
        typedef ::Tools::Color3f Color3f;

    private:
        Color3f _backgroundColor;

    public:
        Viewport(std::string const& className = "",
                 std::string const& id = "");
        virtual void OnExpose(Events::Expose const& evt);
        virtual void OnResize(Events::Resize const& evt);
        virtual void Render(IRenderer& r);
        virtual std::string const& GetName() const
        {
            static std::string name = "viewport";
            return name;
        }
    protected:
        void _Resize(size_t w, size_t h);
    };

}}

#endif
