#ifndef __TOOLS_GUI_WIDGETFACTORY_HPP__
#define __TOOLS_GUI_WIDGETFACTORY_HPP__

namespace Tools { namespace Gui {

    struct WidgetFactory
    {
        template<typename T>
        static std::unique_ptr<T> Build()
        {
            return std::unique_ptr<T>(new T());
        }
        template<typename T, typename P1>
        static std::unique_ptr<T> Build(P1 p1)
        {
            return std::unique_ptr<T>(new T(p1));
        }
        template<typename T, typename P1, typename P2>
        static std::unique_ptr<T> Build(P1 p1, P2 p2)
        {
            return std::unique_ptr<T>(new T(p1, p2));
        }
        template<typename T, typename P1, typename P2, typename P3>
        static std::unique_ptr<T> Build(P1 p1, P2 p2, P3 p3)
        {
            return std::unique_ptr<T>(new T(p1, p2, p3));
        }
    };

}}

#endif
