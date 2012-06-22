#include "client/game/engine/DoodadManager.hpp"
#include "client/game/engine/ModelManager.hpp"
#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/DoodadType.hpp"
#include "client/game/engine/Engine.hpp"
#include "client/game/ShapeRenderer.hpp"
#include "tools/lua/Interpreter.hpp"
#include "common/FieldUtils.hpp"
#include "common/physics/World.hpp"
#include "bullet/bullet-all.hpp"

namespace Client { namespace Game { namespace Engine {

    DoodadManager::DoodadManager(Engine& engine) :
        _engine(engine),
        _runningDoodadId(0),
        _runningDoodad(0),
        _lastTime(0),
        _world(0),
        _shapeRenderer(0)
    {
        this->_world = new Common::Physics::World();
        this->_shapeRenderer = new ShapeRenderer(this->_engine.GetGame());
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals().GetTable("Client").GetTable("Doodad");
        namespaceTable.Set("Register", i.MakeFunction(std::bind(&DoodadManager::_ApiRegister, this, std::placeholders::_1)));
    }

    DoodadManager::~DoodadManager()
    {
        // doodads
        auto itDoodad = this->_doodads.begin();
        auto itDoodadEnd = this->_doodads.end();
        for (; itDoodad != itDoodadEnd; ++itDoodad)
            Tools::Delete(itDoodad->second);
        // doodad types
        auto itPlugin = this->_doodadTypes.begin();
        auto itPluginEnd = this->_doodadTypes.end();
        for (; itPlugin != itPluginEnd; ++itPlugin)
            Tools::Delete(itPlugin->second);

        Tools::Delete(this->_world);
        Tools::Delete(this->_shapeRenderer);
    }

    //Uint32 DoodadManager::GetRunningPluginId() const
    //{
    //    return this->_runningDoodad ? this->_runningDoodad->GetType().GetPluginId() : 0;
    //}

    Doodad const& DoodadManager::GetDoodad(Uint32 doodadId) const throw(std::runtime_error)
    {
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end() || !it->second)
            throw std::runtime_error("DoodadManager::GetDoodad: Doodad not found.");
        return *it->second;
    }

    void DoodadManager::Tick(Uint64 totalTime)
    {
        this->_world->Tick(totalTime - this->_lastTime);//stepSimulation(deltaTime);

        auto it = this->_doodads.begin();
        auto itEnd = this->_doodads.end();
        for (; it != itEnd; ++it)
        {
            Doodad& entity = *it->second;
            //btVector3 const& btPos = entity.GetBtBody().getCenterOfMassPosition();
            Common::Physics::Node& physics = entity.GetPhysics();

            btTransform wt;
            entity.GetBtBody().getMotionState()->getWorldTransform(wt);
            btVector3 wpos = wt.getOrigin();

            physics.position.x = wpos.x();
            physics.position.y = wpos.y();
            physics.position.z = wpos.z();

            btQuaternion wrot = wt.getRotation();
            glm::quat glmRot(wrot.w(), wrot.x(), wrot.y(), wrot.z());
            physics.orientation = //glm::eulerAngles(glmRot);
            glmRot;

            float uf = it->second->GetUpdateFlag();
            uf -= 0.3;
            if (uf < 0)
                uf = 0;
            it->second->SetUpdateFlag(uf);

            //Common::Physics::MoveNode(it->second->GetPhysics(), deltaTime);
            this->_CallDoodadFunction(it->first, "Think");
        }
        this->_lastTime = totalTime;
    }

    void DoodadManager::SpawnDoodad(Uint32 doodadId,
            std::string const& doodadName,
            Common::Physics::Node const& position,
            std::list<std::pair<std::string /* key */, std::string /* value */>> const& values)
    {
        if (this->_doodads.count(doodadId))
        {
            Tools::error << "DoodadManager::SpawnDoodad: Doodad " << doodadId << " already exists." << std::endl;
            return;
        }
        auto it = this->_doodadTypes.find(doodadName);
        if (it == this->_doodadTypes.end())
        {
            Tools::error << "DoodadManager::SpawnDoodad: Doodad type \"" << doodadName << "\" not found." << std::endl;
            return;
        }
        this->_doodads[doodadId] = new Doodad(this->_engine.GetInterpreter(), doodadId, position, *it->second);
        auto itValues = values.begin();
        auto itValuesEnd = values.end();
        for (; itValues != itValuesEnd; ++itValues)
        {
            auto const& v = *itValues;
            // TODO
        }
        this->_world->GetBtWorld().addRigidBody(&this->_doodads[doodadId]->GetBtBody());
        this->_CallDoodadFunction(doodadId, "Spawn");
    }

    void DoodadManager::KillDoodad(Uint32 doodadId)
    {
        auto it = this->_doodads.find(doodadId);
        if (it != this->_doodads.end())
        {
            this->_CallDoodadFunction(doodadId, "Die");
            this->_engine.GetModelManager().DeleteModelsOfDoodad(doodadId);
            Tools::Delete(it->second);
            this->_doodads.erase(it);
        }
        else
            Tools::error << "DoodadManager::KillDoodad: Doodad " << doodadId << " does not exist." << std::endl;
    }

    void DoodadManager::UpdateDoodad(Uint32 doodadId,
            Common::Physics::Node const* position,
            std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>> const& commands)
    {
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end())
        {
            Tools::error << "DoodadManager::UpdateDoodad: Doodad " << doodadId << " not found." << std::endl;
            return;
        }
        if (position)
        {
            it->second->SetPhysics(*position);

            btRigidBody& btBody = it->second->GetBtBody();


            btTransform wt;
            wt.setOrigin(btVector3(position->position.x,
                                     position->position.y,
                                     position->position.z));
            wt.setRotation(btQuaternion(
                        position->orientation.x,
                        position->orientation.y,
                        position->orientation.z,
                        position->orientation.w));

            btBody.getMotionState()->setWorldTransform(wt);


            btBody.setLinearVelocity(
                    btVector3(position->velocity.x,
                        position->velocity.y,
                        position->velocity.z));

            btBody.setAngularVelocity(
                    btVector3(position->angularVelocity.x,
                        position->angularVelocity.y,
                        position->angularVelocity.z));


            it->second->SetUpdateFlag(1.1f);
        }

        auto itCommands = commands.begin();
        auto itCommandsEnd = commands.end();
        for (; itCommands != itCommandsEnd; ++itCommands)
        {
            auto const& c = *itCommands;
            if (std::get<0>(c)) // functionCall
            {
                this->_CallDoodadFunction(doodadId, std::get<1>(c) /* TODO value */);
            }
            else // pas functionCall
            {
                // TODO
            }
        }
    }

    void DoodadManager::Render()
    {
        for (auto it = this->_doodads.begin(), ite = this->_doodads.end(); it != ite; ++it)
        {
            this->_shapeRenderer->Render(it->second->GetBtBody());
        }
    }

    void DoodadManager::_CallDoodadFunction(Uint32 doodadId, std::string const& function)
    {
        assert(!this->_runningDoodadId && !this->_engine.GetRunningDoodadId() && "chainage de calls Lua, THIS IS BAD");
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end())
        {
            Tools::error << "DoodadManager::_CallDoodadFunction: Call to \"" << function << "\" for doodad " << doodadId << " failed: doodad not found.\n";
            return;
        }
        try
        {
            auto prototype = it->second->GetType().GetPrototype();
            if (!prototype.IsTable())
                throw std::runtime_error("prototype is not a table"); // le moddeur a fait de la merde avec son type...
            auto f = it->second->GetType().GetPrototype()[function];
            if (f.IsFunction())
            {
                this->_runningDoodadId = doodadId;
                this->_runningDoodad = it->second;
                f();
                this->_runningDoodad = 0;
                this->_runningDoodadId = 0;
            }
            else
            {
                Tools::error << "DoodadManager::_CallDoodadFunction: Call to \"" << function << "\" for doodad " << doodadId << " (\"" << it->second->GetType().GetName() << "\") failed: function not found (type: " << f.GetTypeName() << ").\n";
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "DoodadManager::_CallDoodadFunction: Fatal (doodad deleted): Call to \"" << function << "\" for doodad " << doodadId << " (\"" << it->second->GetType().GetName() << "\") failed: " << e.what() << std::endl;
            // TODO delete doodad
            this->_runningDoodad = 0;
            this->_runningDoodadId = 0;
            return;
        }
    }

    void DoodadManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        std::string const& pluginName = this->_engine.GetRunningPluginName();
        if (pluginName == "")
            throw std::runtime_error("Client.Doodad.Register: Could not determine currently running plugin, aborting registration");
        Tools::Lua::Ref prototype(this->_engine.GetInterpreter().GetState());
        std::string doodadName;
        try
        {
            prototype = helper.PopArg("Client.Doodad.Register: Missing argument \"prototype\"");
            if (!prototype.IsTable())
                throw std::runtime_error("Client.Doodad.Register: Argument \"prototype\" must be a table");
            if (!prototype["doodadName"].IsString())
                throw std::runtime_error("Client.Doodad.Register: Field \"doodadName\" must exist and be a string");
            if (!Common::FieldUtils::IsRegistrableType(doodadName = prototype["doodadName"].ToString()))
                throw std::runtime_error("Client.Doodad.Register: Invalid doodad name \"" + doodadName + "\"");
            doodadName = Common::FieldUtils::GetResourceName(pluginName, doodadName);
            if (this->_doodadTypes.count(doodadName))
                throw std::runtime_error("Client.Doodad.Register: A doodad with the name \"" + doodadName + "\" is already registered");
        }
        catch (std::exception& e)
        {
            Tools::error << "DoodadManager::_ApiRegister: Failed to register new doodad type from plugin " << pluginName << " (file: " << this->_engine.GetRunningResourceName() << "): " << e.what() << std::endl;
            return;
        }
        this->_doodadTypes[doodadName] = new DoodadType(doodadName, prototype);
        Tools::debug << "Doodad \"" << doodadName << "\" registered." << std::endl;
    }

}}}
