#ifndef __CLIENT_RESOURCES_EFFECT_HPP__
#define __CLIENT_RESOURCES_EFFECT_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Lua {
    class CallHelper;
    class Ref;
}}

namespace Client { namespace Resources {

    class Effect
    {
    private:
        Uint32 _pluginId;
        std::string _name;
        Tools::Renderers::IShaderProgram* _shader;
        std::map<std::string, std::unique_ptr<Tools::Renderers::IShaderParameter>> _parameters;
        Tools::Lua::Ref* _settings;
        Tools::Lua::Ref* _initObject;
        Tools::Lua::Ref* _update;

    public:
        Effect(Game::Game& game, Tools::Lua::Ref settings, Uint32 pluginId);
        ~Effect();

        void Init(Tools::Lua::Ref const& object);
        void Update(Tools::Lua::Ref const& object);

        std::string const& GetName() const { return this->_name; }

    private:
        Effect(Effect const&);
        Effect& operator =(Effect const&);
    };

}}

#endif
