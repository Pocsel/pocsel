#include "tools/IRenderer.hpp"

#include "tools/gui/events/all.hpp"

#include "tools/gui/Widget.hpp"

namespace Tools { namespace Gui {

    namespace {

        Stylesheet _GetDefaultStylesheet()
        {
            Stylesheet stylesheet;
            stylesheet.SetStyle("font-size", "12px");
            // XXX mettre un systeme de ouf qui recherche les fonts
#ifdef __linux__
            stylesheet.SetStyle("font-file", "/usr/share/fonts/truetype/freefont/FreeMono.ttf");
#else
            stylesheet.SetStyle("font-file", "C:/Windows/Fonts/Comic sans ms.ttf");
#endif
            stylesheet.SetStyle("font-color", "black");
            stylesheet.SetStyle("background-color", "transparent");
            stylesheet.Dump();
            return stylesheet;
        }

        std::string GenId(std::string const& widget_name, std::string const& id)
        {
            static std::set<std::string> registered_ids;
            static std::unordered_map<std::string, size_t> widget_ids;
            if (id.size() > 0)
            {
                if (registered_ids.find(id) != registered_ids.end())
                    throw std::runtime_error(
                                             "The widget name '" + id + "' is already used"
                                            );
                registered_ids.insert(id);
                return id;
            }
            std::string new_name = widget_name + Tools::ToString(++widget_ids[widget_name]);
            registered_ids.insert(new_name);
            return new_name;
        }

    } // !anonymous

    Stylesheet Widget::_defaultStylesheet = _GetDefaultStylesheet();

    Widget::Widget(std::string const& name,
                   std::string const& className,
                   std::string const& id) :
        _name(name),
        _className(className),
        _id(GenId(name, id)),
        _geometry(),
        _parent(),
        _sizePolicy(),
        _stylesheet(&Widget::_defaultStylesheet),
        _computedStyles(*this)
    {
#ifdef DEBUG
        std::cout << "Creating " << name << " widget (" << _id << ")\n";
#endif
    }

    Widget::~Widget()
    {}

    std::string const& Widget::style(std::string const& name)
    {
        return this->_stylesheet->GetStyle(
                                           name,
                                           this->_name,
                                           this->_className,
                                           this->_id
                                          );
    }

    void Widget::style(std::string const& name, std::string const& value)
    {
        this->_stylesheet->SetStyle(
                                    name, value,
                                    this->_name,
                                    this->_className,
                                    this->_id
                                   );
    }

    void Widget::ReloadStyles()
    {
        std::cout << "reload styles of " << this->_id << std::endl;
        this->_computedStyles.Reload();
    }

    void Widget::Render(Tools::IRenderer&)
    {
        //auto bg_color = this->styles().GetBackgroundColor();
        //    glBegin(GL_QUADS);
        //    glColor4f(
        //        bg_color.r,
        //        bg_color.g,
        //        bg_color.b,
        //        bg_color.a
        //    );
        //    glVertex2f(this->pos().x, this->pos().y);
        //    glVertex2f(this->pos().x + this->size().w, this->pos().y);
        //    glVertex2f(this->pos().x + this->size().w, this->pos().y + this->size().h);
        //    glVertex2f(this->pos().x, this->pos().y + this->size().h);
        //    glEnd();
    }

    void Widget::OnResize(Events::Resize const& evt)
    {
        this->size(evt.width, evt.height);
    }
    //
    //void Widget::fire(std::string const& str, Event const& event)
    //{
    //    if (this->_parent != 0)
    //        this->_parent->fire(str, event);
    //}
    //
    //void Widget::fireSpecial(std::string const& str, Event const& event)
    //{
    //    if (this->_parent != 0)
    //    {
    //        std::string newString = this->name();
    //        newString += "/";
    //        newString += str;
    //        this->_parent->fireSpecial(newString, event);
    //    }
    //}
    //
    //void Widget::connect(std::string const& str,
    //                     std::function<bool(Event const&)> callback)
    //{
    //    if (this->_parent != 0)
    //    {
    //        std::string newString = this->name();
    //        newString += "/";
    //        newString += str;
    //        this->_parent->connect(newString, callback);
    //    }
    //}

}}
