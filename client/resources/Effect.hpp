#ifndef __CLIENT_RESOURCES_EFFECT_HPP__
#define __CLIENT_RESOURCES_EFFECT_HPP__

#include "tools/IRenderer.hpp"
#include "client/game/Game.hpp"

namespace Tools { namespace Lua {
    class CallHelper;
    class Ref;
}}

namespace Client { namespace Resources {

    class Effect
    {
    private:
        std::string _name;
        Tools::Renderers::IShaderProgram* _shader;
        std::map<std::string, std::unique_ptr<Tools::Renderers::IShaderParameter>> _parameters;
        std::unique_ptr<Tools::Lua::Ref> _settings;
        std::unique_ptr<Tools::Lua::Ref> _initObject;
        std::unique_ptr<Tools::Lua::Ref> _update;

    public:
        Effect(Game::Game& game, Tools::Lua::Ref settings);

        void Init(Tools::Lua::Ref const& object);
        void Update(Tools::Lua::Ref const& object);

        void BeginPass() { this->_shader->BeginPass(); }
        bool EndPass() { return this->_shader->EndPass(); }
        Tools::Renderers::IShaderParameter& GetParameter(std::string const& name) { return this->_shader->GetParameter(name); }

        std::string const& GetName() const { return this->_name; }

    private:
        Effect(Effect const&);
        Effect& operator =(Effect const&);
    };

    // TODO !
    //class EffectState
    //{
    //public:
    //    ~EffectState() {}
    //    void Set(Effect& effect);
    //};

}}

#endif
