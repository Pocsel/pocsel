#ifndef __TOOLS_GUI_SETTINGS_HPP__
#define __TOOLS_GUI_SETTINGS_HPP__

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace Tools { namespace Gui {

    class Settings
    {
    private:
        std::unordered_map<std::string, std::string> _settings;

    public:
        std::string& operator [](std::string const& key)
        {
            return this->_settings[key];
        }

        std::string const& operator [](std::string const& key) const
        {
            auto it = this->_settings.find(key);
            if (it == this->_settings.end())
                throw std::runtime_error("KeyError: '" + key + "' not found");
            return it->second;
        }

    };

}}

#endif
