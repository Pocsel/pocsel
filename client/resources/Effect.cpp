#include "client/precompiled.hpp"

#include "tools/logger/Logger.hpp"
#include "tools/lua/Ref.hpp"
#include "common/FieldValidator.hpp"

#include "client/game/Game.hpp"
#include "client/resources/Effect.hpp"

namespace Client { namespace Resources {

    Effect::Effect(Game::Game& game, Tools::Lua::Ref settings, Uint32 pluginId)
        : _pluginId(pluginId),
        _shader(0),
        _settings(new Tools::Lua::Ref(settings)),
        _initObject(0),
        _update(0)
    {
        try
        {
            this->_name = settings["effectName"].CheckString("Client.Effect.Register: Field \"effectName\" must exist and be a string");
            if (!Common::FieldValidator::IsRegistrableType(this->_name))
                throw std::runtime_error("Client.Effect.Register: Invalid effect name \"" + this->_name + "\"");
            this->_shader = &game.GetResourceManager().GetShader(pluginId, settings["resource"].CheckString("Client.Effect.Register: Field \"resource\" must exist and be a string"));
            if (settings["initObject"].IsFunction())
                this->_initObject = new Tools::Lua::Ref(settings["initObject"]);
            if (settings["update"].IsFunction())
                this->_update = new Tools::Lua::Ref(settings["update"]);
        }
        catch (std::exception& e)
        {
            Tools::error << "Failed to load an effect from cache, check your plugin or cache file: " << e.what() << "\n";
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
        //if (this->_initObject != 0)
        //    (*this->_initObject)(*this->_settings, object);
    }

    void Effect::Update(Tools::Lua::Ref const& object)
    {
        //if (this->_update != 0)
        //    (*this->_update)(*this->_settings, object);
    }
}}
