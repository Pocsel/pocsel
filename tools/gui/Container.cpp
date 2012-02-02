
#include <iostream>

#include "events/all.hpp"

#include "Container.hpp"

using namespace Tools::Gui;

Container::Container(std::string const& name,
                     std::string const& className,
                     std::string const& id) :
    Widget(name, className, id)
{
}

Container::~Container()
{
    auto it = this->_childs.begin(),
         end = this->_childs.end();
    for (; it != end; ++it)
        delete (*it);
}

void Container::ReloadStyles()
{
    Widget::ReloadStyles();
    auto it = this->_childs.begin(),
         end = this->_childs.end();
    for (; it != end; ++it)
        (*it)->ReloadStyles();
}


void Container::_AddChild(Widget* widget)
{
    std::cout << "Adding child " <<  widget->name() << " in "
              << this->name() << std::endl;
    widget->parent(this);
    this->_childs.push_back(widget);
}

void Container::OnMouseMove(Events::MouseMove const& event)
{
    for (auto it = this->_childs.begin(),
         ite = this->_childs.end() ; it != ite ; ++it)
        if ((*it)->geometry().Contains(event.x, event.y))
        {
            (*it)->OnMouseMove(event);
            return;
        }
}

void Container::OnMouseButton(Events::MouseButton const& event)
{
    for (auto it = this->_childs.begin(),
         ite = this->_childs.end() ; it != ite ; ++it)
        if ((*it)->geometry().Contains(event.x, event.y))
        {
            (*it)->OnMouseButton(event);
            return;
        }
}

void Container::OnResize(Events::Resize const& evt)
{
    auto it = this->_childs.begin(),
         end = this->_childs.end();
    for (; it != end; ++it)
        (*it)->OnResize(evt);
}

void Container::OnKey(Events::Key const& evt)
{
    auto it = this->_childs.begin(),
         end = this->_childs.end();
    for (; it != end; ++it)
        (*it)->OnKey(evt);
}

