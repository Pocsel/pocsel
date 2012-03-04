#ifndef __SERVER_GAME_PLUGINMANAGER_HPP__
#define __SERVER_GAME_PLUGINMANAGER_HPP__

namespace Server { namespace Game {
    class PluginManager :
        private boost::noncopyable
    {
    public:
        struct Plugin
        {
            std::string fullname;
            std::string identifier;
        };

    private:
        std::map<Uint32, Plugin> _plugins;

    public:
        PluginManager();
        void AddPlugin(Uint32 id, std::string const& fullname, std::string const& identifier) throw(std::runtime_error);
        Plugin const& GetPlugin(Uint32 id) const throw(std::runtime_error);
        std::map<Uint32, Plugin> const& GetPlugins() const { return this->_plugins; }
        std::string GetPluginFullname(Uint32 id) const;
        std::string GetPluginIdentifier(Uint32 id) const;
    };

}}

#endif
