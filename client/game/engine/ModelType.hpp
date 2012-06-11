#ifndef __CLIENT_GAME_ENGINE_MODELTYPE_HPP__
#define __CLIENT_GAME_ENGINE_MODELTYPE_HPP__

namespace Tools { namespace Lua { 
    class Ref;
}}

namespace Client { namespace Resources {
    class ResourceManager;
}}

namespace Client { namespace Game { namespace Engine {

    class ModelType :
        private boost::noncopyable
    {
    private:
        std::string _name;
        Uint32 _resourceId;

    public:
        ModelType(std::string const& name, Uint32 resourceId);
        std::string const& GetName() const { return this->_name; }
        Uint32 GetResourceId() const { return this->_resourceId; }
    };

}}}

#endif
