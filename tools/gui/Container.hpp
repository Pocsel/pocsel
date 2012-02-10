#ifndef __TOOLS_GUI_CONTAINER_HPP__
#define __TOOLS_GUI_CONTAINER_HPP__

#include "tools/gui/Widget.hpp"

namespace Tools { namespace Gui {

    class Container : public Widget
    {
    protected:
        std::vector<Widget*> _childs;

    protected:
        Container(std::string const& name,
                  std::string const& className = "",
                  std::string const& id = "");
        virtual ~Container();
    public:
        virtual void ReloadStyles();
        template<typename T>
        T& AddChild(std::unique_ptr<T> child)
        {
            T* c = child.release();
            assert(c != 0 && "Add null child");
            this->_AddChild(c);
            return *c;
        }
        virtual void OnMouseMove(Events::MouseMove const& event);
        virtual void OnMouseButton(Events::MouseButton const& event);
        virtual void OnResize(Events::Resize const& evt);
        virtual void OnKey(Events::Key const& evt);

    public: // getter / setter
        virtual Widget& GetChild(size_t idx) { return *this->_childs[idx]; }

    protected:
        virtual void _AddChild(Widget* widget);
    };

}}

#endif
