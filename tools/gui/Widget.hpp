#ifndef __TOOLS_GUI_WIDGET_HPP__
#define __TOOLS_GUI_WIDGET_HPP__

#include <string>

#include <boost/noncopyable.hpp>

#include "tools/IRenderer.hpp"
#include "tools/Rectangle.hpp"
#include "tools/Vector2.hpp"

#include "events/all_forward.hpp"

#include "ComputedStyles.hpp"
#include "Event.hpp"
#include "SizePolicy.hpp"
#include "Stylesheet.hpp"

namespace Tools { namespace Gui {

    class Container;
    class Font;

    class Widget : private boost::noncopyable
    {
    public:
        typedef unsigned int HandlerId;
        typedef std::function<bool(Event const&)> EventHandler;

    private:
        std::string         _name;
        std::string         _className;
        std::string         _id;
        Rectangle           _geometry;
        Container*          _parent;
        SizePolicy          _sizePolicy;
        Stylesheet*         _stylesheet;
        ComputedStyles      _computedStyles;
        static Stylesheet   _defaultStylesheet;

    protected:
        Widget(std::string const& name,
               std::string const& className = "",
               std::string const& id = "");

    public:
        virtual ~Widget();
        virtual void ReloadStyles();
        virtual void Render(Tools::IRenderer& r);
        //virtual void fire(std::string const& str, Event const& event);
        //virtual void fireSpecial(std::string const& str, Event const& event);
        virtual HandlerId Connect(std::string const&,
                                  std::function<bool(Event const&)>) { return 0; }
        virtual void Disconnect(HandlerId) {}

    public:
        virtual void OnMouseButton(Events::MouseButton const&) {}
        virtual void OnExpose(Events::Expose const&) {}
        virtual void OnIdle(Events::Idle const&) {}
        virtual void OnKey(Events::Key const&) {}
        virtual void OnMouseMove(Events::MouseMove const&) {}
        virtual void OnQuit(Events::Quit const&) {}
        virtual void OnEvent(Event const&) {}
        virtual void OnResize(Events::Resize const&);

    public: // getters / setters
        virtual std::string const& name() const { return this->_name; }
        inline std::string const& id() const { return this->_id; }
        inline Vector2i& pos() { return this->_geometry.pos; }
        inline Vector2i const& pos() const { return this->_geometry.pos; }
        inline void pos(Vector2i const& p) { this->pos(p.x, p.y); }
        inline void pos(int x, int y) { this->_geometry.pos.x = x; this->_geometry.pos.y = y; }

        inline Vector2u& size() { return this->_geometry.size; }
        inline Vector2u const& size() const { return this->_geometry.size; }

        inline void size(unsigned w, unsigned h) {this->_geometry.size.w = w; this->_geometry.size.h = h;}
        inline void size(Vector2u const& s) { this->size(s.w, s.h); }

        inline void parent(Container* parent) { this->_parent = parent; }
        inline Container* parent() { return this->_parent; }

        inline SizePolicy const& sizePolicy() const { return this->_sizePolicy; }
        inline SizePolicy& sizePolicy() { return this->_sizePolicy; }

        inline Rectangle const& geometry() const { return this->_geometry; }
        inline Rectangle& geometry() { return this->_geometry; }

        inline void sizePolicy(SizePolicy::Constraint constraint)
        {
            this->_sizePolicy.widthConstraint = constraint;
            this->_sizePolicy.heightConstraint = constraint;
        }
        inline void sizePolicy(SizePolicy::Unit unit)
        {
            this->_sizePolicy.widthUnit = unit;
            this->_sizePolicy.heightUnit = unit;
        }
        inline void sizePolicy(unsigned int width, unsigned int height)
        {
            this->_sizePolicy.width = width;
            this->_sizePolicy.height = height;
        }
        inline Stylesheet& stylesheet() { return *this->_stylesheet; }
        inline void stylesheet(Stylesheet& stylesheet) { this->_stylesheet = &stylesheet; }
        inline Font& font() const
        {
            return this->_stylesheet->GetFont(
                this->_name,
                this->_className,
                this->_id
            );
        }
        inline ComputedStyles const& styles() const { return this->_computedStyles; }

        std::string const& style(std::string const& name);
        void style(std::string const& name, std::string const& value);

        static void defaultStylesheet(Stylesheet& stylesheet)
        {
            Widget::_defaultStylesheet = stylesheet;
        }
        static Stylesheet& defaultStylesheet() { return Widget::_defaultStylesheet; }
    };

}}

#endif
