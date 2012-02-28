#include "client/precompiled.hpp"

#include "tools/logger/Logger.hpp"
#include "tools/lua/Ref.hpp"

#include "client/game/Game.hpp"
#include "client/resources/Effect.hpp"

namespace Client { namespace Resources {

    Effect::Effect(Game::Game& game, Tools::Lua::CallHelper& helper, Uint32 pluginId)
        : _pluginId(pluginId),
        _shader(0),
        _settings(0),
        _initObject(0),
        _update(0)
    {
        try
        {
            auto settings = helper.PopArg();
            // TODO: id
            this->_shader = &game.GetResourceManager().GetShader(pluginId, settings["resource"].CheckString());
            if (settings["initObject"].IsFunction())
                this->_initObject = new Tools::Lua::Ref(settings["initObject"]);
            if (settings["update"].IsFunction())
                this->_update = new Tools::Lua::Ref(settings["update"]);
            this->_settings = new Tools::Lua::Ref(settings);
        }
        catch (std::exception& e)
        {
            Tools::error << "Fail to load an effect from cache, check your plugin or tccache file:\n" << e.what() << "\n";
            Tools::Delete(this->_settings);
            Tools::Delete(this->_initObject);
            Tools::Delete(this->_update);
            throw;
        }
    }

    Effect::~Effect()
    {
        Tools::Delete(this->_settings);
        Tools::Delete(this->_initObject);
        Tools::Delete(this->_update);
    }

    void Effect::Init(Tools::Lua::Ref const& object)
    {
        if (this->_initObject == 0)
            return;
        (*this->_initObject)(*this->_settings, object);
    }

    void Effect::Update(Tools::Lua::Ref const& object)
    {
        if (this->_update == 0)
            return;
        (*this->_update)(*this->_settings, object);
    }

}}
