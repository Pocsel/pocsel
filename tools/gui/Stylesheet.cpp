#include "tools/gui/Stylesheet.hpp"
#include "tools/gui/Font.hpp"

namespace Tools { namespace Gui {

    Stylesheet::Stylesheet() :
        _definitions(),
        _fonts()
    {
    }

    Stylesheet::Stylesheet(Stylesheet const& other) :
        _definitions(other._definitions),
        _fonts()
    {
    }

    Stylesheet::~Stylesheet()
    {
        auto it = this->_fonts.begin(),
        end = this->_fonts.end();
        for (; it != end; ++it)
            delete it->second;
    }

    Stylesheet& Stylesheet::operator =(Stylesheet const& other)
    {
        if (this != &other)
        {
            this->_definitions = other._definitions;
            auto it = this->_fonts.begin(),
                 end = this->_fonts.end();
            for (; it != end; ++it)
                delete it->second;
            this->_fonts.clear();
        }
        return *this;
    }


    std::string const& Stylesheet::GetStyle(std::string const& style_name,
                                            std::string const& widget_name,
                                            std::string const& class_name,
                                            std::string const& name) const
    {
        static std::string empty;
        int best_match = -1;
        std::string const* s = &empty;

        auto it = this->_definitions.rbegin(),
             end = this->_definitions.rend();
        for (; it != end; ++it)
        {
            int match = 0;
            StylesDefinition const& def = *it;

            if (def.widget_name.size() == 0) ;
            else if (def.widget_name == widget_name) match += 1;
            else continue;

            if (def.class_name.size() == 0) ;
            else if (def.class_name == class_name) match += 2;
            else continue;

            if (def.name.size() == 0) ;
            else if (def.name == name) match += 4;
            else continue;

            if (match <= best_match) continue;

            auto style_it = def.find(style_name);
            if (style_it != def.end())
            {
                s = &(style_it->second);
                best_match = match;
                if (match >= 4)
                    break;
            }
        }
        return *s;
    }

    void Stylesheet::SetStyle(std::string const& style_name,
                              std::string const& value,
                              std::string const& widget_name,
                              std::string const& class_name,
                              std::string const& name)
    {
        StylesDefinition def(widget_name, class_name, name);
        def[style_name] = value;
        this->_definitions.push_back(def);
    }

    size_t Stylesheet::GetPixelSize(std::string const& style)
    {
        if (style.size() == 0)
            return 0;
        size_t idx;
        if ((idx = style.find("px")) != -1)
        {
            std::stringstream ss(style);
            size_t size;
            ss >> size;
            return size;
        }
        throw std::runtime_error("Unknown size format for '" + style + "'");
    }


    Font& Stylesheet::GetFont(std::string const& widget_name,
                              std::string const& class_name,
                              std::string const& name)
    {
        FontDefinition fd(
                          this->GetStyle("font-file", widget_name, class_name, name),
                          this->GetStyle("font-color", widget_name, class_name, name),
                          Stylesheet::GetPixelSize(this->GetStyle("font-size", widget_name, class_name, name))
                         );

        auto font_it = this->_fonts.find(fd);
        Font* font;
        if (font_it == this->_fonts.end())
        {
            font = new Font(fd.font_file, fd.font_size);
            this->_fonts[fd] = font;
        }
        else
            font = font_it->second;
        if (font == 0)
            throw std::runtime_error("Corrupted stylesheet");
        return *font;
    }

    void Stylesheet::Dump() const
    {
        std::cout << "----------- Dumping style sheet ("
            << this->_definitions.size() << ")" << std::endl;

        auto it = this->_definitions.begin(),
             end = this->_definitions.end();
        for (; it != end; ++it)
        {
            StylesDefinition const& def = *it;
            std::cout << " - definition" << def.widget_name << ' '
                <<  def.class_name << ' ' << def.name << std::endl;
            auto style_it = def.begin(),
                 style_end = def.end();
            for (; style_it != style_end; ++style_it)
            {
                std::cout << "\t" << style_it->first << ":" << style_it->second << std::endl;
            }
        }
    }

}}
