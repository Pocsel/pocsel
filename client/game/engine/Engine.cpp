#include "client/game/engine/Engine.hpp"

namespace Client { namespace Game { namespace Engine {

    Engine::Engine(Game& game) :
        _game(game), _overriddenPluginId(0), _overriddenDoodadId(0)
    {
        this->_interpreter = new Tools::Lua::Interpreter();
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Base);
        this->_interpreter->Globals().Set("print", this->_interpreter->MakeFunction(std::bind(&Engine::_ApiPrint, this, std::placeholders::_1)));
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Math);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Table);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::String);
        Tools::Lua::Utils::RegisterColor(this->_interpreter);
        this->_doodadManager = new DoodadManager(*this);
    }

    Engine::~Engine()
    {
        Tools::Delete(this->_doodadManager);
        Tools::Delete(this->_interpreter);
    }

    void Engine::_ApiPrint(Tools::Lua::CallHelper& helper)
    {
        std::string str = "[";
        if (this->GetRunningPluginId())
            str += Tools::ToString(this->GetRunningPluginId());
        else
            str += "?";
        str += "/";
        try
        {
            auto& d = this->_doodadManager->GetDoodad(this->GetRunningDoodadId());
            str += d.GetType().GetName() + "/" + Tools::ToString(this->GetRunningDoodadId());
        }
        catch (std::exception&)
        {
            str += "?";
        }
        str += "]";
        auto it = helper.GetArgList().begin();
        auto itEnd = helper.GetArgList().end();
        for (; it != itEnd; ++it)
            str += it->ToString();
        Tools::log << str << std::endl;
    }

}}}
