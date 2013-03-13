#include "server/precompiled.hpp"

#include "server/game/engine/MessageManager.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "server/game/engine/Entity.hpp"
#include "server/game/engine/EntityType.hpp"
#include "tools/database/IConnection.hpp"
#include "server/game/map/Map.hpp"

namespace Server { namespace Game { namespace Engine {

    MessageManager::MessageManager(Engine& engine) :
        _engine(engine)
    {
        Tools::debug << "MessageManager::MessageManager()\n";
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals()["Server"].Set("Message", i.MakeTable());
        namespaceTable.Set("Later", i.MakeFunction(std::bind(&MessageManager::_ApiLater, this, std::placeholders::_1)));
        namespaceTable.Set("Now", i.MakeFunction(std::bind(&MessageManager::_ApiNow, this, std::placeholders::_1)));
    }

    MessageManager::~MessageManager()
    {
        Tools::debug << "MessageManager::~MessageManager()\n";
        auto it = this->_messages.begin();
        auto itEnd = this->_messages.end();
        for (; it != itEnd; ++it)
        {
            auto itMessage = it->second.begin();
            auto itMessageEnd = it->second.end();
            for (; itMessage != itMessageEnd; ++itMessage)
                Tools::Delete(*itMessage);
        }
    }

    void MessageManager::DispatchMessages()
    {
        auto it = this->_messages.begin();
        auto itEnd = this->_messages.end();
        for (; it != itEnd; ++it)
        {
            if (it->first > this->_engine.GetCurrentTime())
                break;
            auto itMessage = it->second.begin();
            auto itMessageEnd = it->second.end();
            for (; itMessage != itMessageEnd; ++itMessage)
            {
                Uint32 entityId = 0;
                try
                {
                    entityId = this->_engine.GetCallbackManager().GetCallback((*itMessage)->callbackId).entityId;
                }
                catch (std::exception&)
                {
                }
                if (entityId)
                {
                    Luasel::Ref ret(this->_engine.GetInterpreter().GetState());
                    CallbackManager::Result res = this->_engine.GetCallbackManager().TriggerCallback((*itMessage)->callbackId, &ret);
                    if ((*itMessage)->notificationCallbackId)
                    {
                        auto resultTable = this->_engine.GetInterpreter().MakeTable();
                        resultTable.Set("entityId", this->_engine.GetInterpreter().MakeNumber(entityId));
                        if (res == CallbackManager::Ok)
                        {
                            resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(true));
                            resultTable.Set("ret", this->_engine.GetInterpreter().GetSerializer().MakeSerializableCopy(ret, true));
                        }
                        else if (res == CallbackManager::Error) // il y a eu une erreur du coté de la cible, mais le message à été envoyé quand meme donc success = true
                            resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(true));
                        else
                            resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(false));
                        this->_engine.GetCallbackManager().TriggerCallback((*itMessage)->notificationCallbackId, resultTable);
                    }
                }
                Tools::Delete(*itMessage);
            }
        }
        this->_messages.erase(this->_messages.begin(), it);
    }

    void MessageManager::_ApiLater(Luasel::CallHelper& helper)
    {
        double seconds = helper.PopArg().CheckNumber("Server.Message.Later: Argument \"seconds\" must be a number");
        if (seconds < 0)
            seconds = 0;
        Uint32 entityId = helper.PopArg("Server.Message.[Later/Now]: Missing argument \"target\"").Check<Uint32>("Server.Message.[Later/Now]: Argument \"target\" must be a number");
        std::string function = helper.PopArg("Server.Message.[Later/Now]: Missing argument \"function\"").CheckString("Server.Message.[Later/Now]: Argument \"function\" must be a string");
        Luasel::Ref arg(this->_engine.GetInterpreter().GetState());
        Uint32 cbTargetId = 0;
        std::string cbFunction;
        Luasel::Ref cbArg(this->_engine.GetInterpreter().GetState());
        if (helper.GetNbArgs())
        {
            arg = helper.PopArg();
            if (helper.GetNbArgs())
            {
                cbTargetId = helper.PopArg("Server.Message.[Later/Now]: Missing argument \"cbTarget\"").Check<Uint32>("Server.Message.[Later/Now]: Argument \"cbTarget\" must be a number");
                cbFunction = helper.PopArg("Server.Message.[Later/Now]: Missing argument \"cbFunction\"").CheckString("Server.Message.[Later/Now]: Argument \"cbFunction\" must be a string");
                if (helper.GetNbArgs())
                    cbArg = helper.PopArg();
            }
        }
        Uint32 callbackId = this->_engine.GetCallbackManager().MakeCallback(entityId, function, arg);
        Uint32 notificationCallbackId = 0;
        if (cbTargetId)
            notificationCallbackId = this->_engine.GetCallbackManager().MakeCallback(cbTargetId, cbFunction, cbArg);
        this->_messages[this->_engine.GetCurrentTime() + static_cast<Uint64>(seconds * 1000000.0)].push_back(new Message(callbackId, notificationCallbackId));
    }

    void MessageManager::_ApiNow(Luasel::CallHelper& helper)
    {
        helper.GetArgList().push_front(this->_engine.GetInterpreter().MakeNumber(0));
        this->_ApiLater(helper);
    }

    void MessageManager::Save(Tools::Database::IConnection& conn)
    {
        std::string table = this->_engine.GetMap().GetName() + "_message";
        conn.CreateQuery("DELETE FROM " + table)->ExecuteNonSelect();
        auto query = conn.CreateQuery("INSERT INTO " + table + " (time, callback_id, notification_callback_id) VALUES (?, ?, ?);");
        auto it = this->_messages.begin();
        auto itEnd = this->_messages.end();
        for (; it != itEnd; ++it)
        {
            auto itMessage = it->second.begin();
            auto itMessageEnd = it->second.end();
            for (; itMessage != itMessageEnd; ++itMessage)
                try
                {
                    Tools::debug << ">> Save >> " << table << " >> Message (time: " << it->first << ", callbackId: " << (*itMessage)->callbackId << ", notificationCallbackId: " << (*itMessage)->notificationCallbackId << ")" << std::endl;
                    query->Bind(it->first).Bind((*itMessage)->callbackId).Bind((*itMessage)->notificationCallbackId).ExecuteNonSelect().Reset();
                }
                catch (std::exception& e)
                {
                    Tools::error << "MessageManager::Save: Could not save message due at " << it->first << ": " << e.what() << std::endl;
                }
        }
    }

    void MessageManager::Load(Tools::Database::IConnection& conn)
    {
        std::string table = this->_engine.GetMap().GetName() + "_message";
        auto query = conn.CreateQuery("SELECT time, callback_id, notification_callback_id FROM " + table);
        while (auto row = query->Fetch())
        {
            Uint64 time = row->GetUint64(0);
            Uint32 callbackId = row->GetUint32(1);
            Uint32 notificationCallbackId = row->GetUint32(2);
            Tools::debug << "<< Load << " << table << " << Message (time: " << time << ", callbackId: " << callbackId << ", notificationCallbackId: " << notificationCallbackId << ")" << std::endl;
            this->_messages[time].push_back(new Message(callbackId, notificationCallbackId));
        }
    }

    // fonctions pour faciliter le RconGetMessages() qui suit
    namespace {

        std::string _GetTimeLeft(Engine& engine, Uint64 time)
        {
            std::stringstream ss;
            ss << std::setprecision(3) << ((time - engine.GetCurrentTime()) / 1000000.0) << " s";
            return ss.str();
        }

        std::string _GetSerializedStringPlusError(Engine& engine, Luasel::Ref const& ref)
        {
            try
            {
                return Rcon::ToJsonStr(engine.GetInterpreter().GetSerializer().Serialize(ref));
            }
            catch (std::exception& e)
            {
                return Rcon::ToJsonStr("Serialization error: " + std::string(e.what()));
            }
        }

        std::string _GetPrettyEntityName(Engine& engine, Uint32 entityId)
        {
            std::string ret = Tools::ToString(entityId) + " (";
            try
            {
                Entity const& e = engine.GetEntityManager().GetEntity(entityId);
                ret += engine.GetWorld().GetPluginManager().GetPluginIdentifier(e.GetType().GetPluginId());
                ret += "/";
                ret += e.GetType().GetName();
            }
            catch (std::exception&)
            {
                ret += "not found";
            }
            ret += ")";
            return Rcon::ToJsonStr(ret);
        }

    }

    std::string MessageManager::RconGetMessages() const
    {
        bool first = true;
        std::string json = "[\n";
        auto itMap = this->_messages.begin();
        auto itMapEnd = this->_messages.end();
        for (; itMap != itMapEnd; ++itMap)
        {
            auto it = itMap->second.begin();
            auto itEnd = itMap->second.end();
            for (; it != itEnd; ++it)
            {
                if (!first)
                    json += ",\n";
                first = false;
                json +=
                    "\t{\n"
                    "\t\t\"seconds\": \"" + _GetTimeLeft(this->_engine, itMap->first) + "\",\n";
                try
                {
                    CallbackManager::Callback const& c = this->_engine.GetCallbackManager().GetCallback((*it)->callbackId);
                    json +=
                        "\t\t\"target\": \"" + _GetPrettyEntityName(this->_engine, c.entityId) + "\",\n" +
                        "\t\t\"function\": \"" + Rcon::ToJsonStr(c.function) + "\",\n" +
                        "\t\t\"argument\": \"" + _GetSerializedStringPlusError(this->_engine, c.arg) + "\",\n";
                }
                catch (std::exception&)
                {
                    json +=
                        "\t\t\"target\": \"?\",\n"
                        "\t\t\"function\": \"?\",\n"
                        "\t\t\"argument\": \"?\",\n";
                }
                try
                {
                    CallbackManager::Callback const& c = this->_engine.GetCallbackManager().GetCallback((*it)->notificationCallbackId);
                    json +=
                        "\t\t\"notification_target\": \"" + _GetPrettyEntityName(this->_engine, c.entityId) + "\",\n" +
                        "\t\t\"notification_function\": \"" + Rcon::ToJsonStr(c.function) + "\",\n" +
                        "\t\t\"notification_argument\": \"" + _GetSerializedStringPlusError(this->_engine, c.arg) + "\"\n";
                }
                catch (std::exception&)
                {
                    // pas de "?" car c'est normal, il n'y a pas toujours de notificationCallback
                    json +=
                        "\t\t\"notification_target\": \"\",\n"
                        "\t\t\"notification_function\": \"\",\n"
                        "\t\t\"notification_argument\": \"\"\n";
                }
                json += "\t}\n";
            }
        }
        json += "\n]\n";
        return json;
    }

}}}

