#ifndef __CLIENT_GAME_ENGINE_MODELTYPE_HPP__
#define __CLIENT_GAME_ENGINE_MODELTYPE_HPP__

namespace Client { namespace Game { namespace Engine {

    class ModelType :
        private boost::noncopyable
    {
    private:
        Uint32 _pluginId;
        std::string _name;
        Uint32 _resourceId;

    public:
        ModelType(Uint32 pluginId, std::string const& name, Uint32 resourceId);
        Uint32 GetPluginId() const { return this->_pluginId; }
        std::string const& GetName() const { return this->_name; }
        Uint32 GetResourceId() const { return this->_resourceId; }
    };

}}}

#endif
