#ifndef __TOOLS_GUI_STYLESHEET_HPP__
#define __TOOLS_GUI_STYLESHEET_HPP__

#include <list>
#include <string>
#include <unordered_map>

#include "tools/Color.hpp"

#include "Font.hpp"

namespace Tools { namespace Gui {

    class StylesDefinition : public std::unordered_map<std::string, std::string>
    {
    public:
        std::string widget_name, class_name, name;

    public:
        StylesDefinition() :
            widget_name(),
            class_name(),
            name()
        {
        }
        StylesDefinition(std::string const& widget_name,
                         std::string const& class_name = "",
                         std::string const& name = "") :
            widget_name(widget_name),
            class_name(class_name),
            name(name)
        {
        }
    };

    struct FontDefinition
    {
        std::string font_file;
        Color4f font_color;
        size_t font_size;

        FontDefinition(std::string const& font_file,
                       Color4f const& color,
                       size_t font_size) :
            font_file(font_file),
            font_color(color),
            font_size(font_size)
        {}

        bool operator ==(FontDefinition const& fd) const
        {
            return (
                this->font_file == fd.font_file &&
                this->font_color == fd.font_color &&
                this->font_size == fd.font_size
            );
        }
    };

}}

namespace std {

    template<> struct hash<Tools::Gui::FontDefinition>
    {
        size_t operator()(Tools::Gui::FontDefinition const& fd) const
        {
            return hash<std::string>()(fd.font_file) + fd.font_size + 1;
        }
    };

} // !std

namespace Tools { namespace Gui {

    class Font;

    class Stylesheet
    {
    private:
        std::list<StylesDefinition> _definitions;
        std::unordered_map<FontDefinition, Font*> _fonts;

    public:
        Stylesheet();
        Stylesheet(Stylesheet const& other);
        virtual ~Stylesheet();
        Stylesheet& operator =(Stylesheet const& other);

        std::string const& GetStyle(std::string const& style_name,
                                 std::string const& widget_name = "",
                                 std::string const& class_name = "",
                                 std::string const& name = "") const;
        void SetStyle(std::string const& style_name,
                   std::string const& value,
                   std::string const& widget_name = "",
                   std::string const& class_name = "",
                   std::string const& name = "");

        Font& GetFont(std::string const& widget_name,
                      std::string const& class_name,
                      std::string const& name);
        void Dump() const;
        static size_t GetPixelSize(std::string const& style);
    };

}}

#endif
