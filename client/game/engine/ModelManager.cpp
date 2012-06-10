#include "client/game/engine/ModelManager.hpp"
#include "client/game/engine/Model.hpp"
#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/ModelType.hpp"
#include "client/game/engine/Engine.hpp"
#include "client/game/Game.hpp"
#include "client/game/ModelRenderer.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Interpreter.hpp"
#include "common/FieldUtils.hpp"

namespace Client { namespace Game { namespace Engine {

    ModelManager::ModelManager(Engine& engine) :
        _engine(engine),
        _nextModelId(1), // le premier model sera le 1, 0 est la valeur spéciale "pas de model"
        _lastTickTime(0)
    {
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals().GetTable("Client").GetTable("Model");
        namespaceTable.Set("Spawn", i.MakeFunction(std::bind(&ModelManager::_ApiSpawn, this, std::placeholders::_1)));
        namespaceTable.Set("Kill", i.MakeFunction(std::bind(&ModelManager::_ApiKill, this, std::placeholders::_1)));
        namespaceTable.Set("Register", i.MakeFunction(std::bind(&ModelManager::_ApiRegister, this, std::placeholders::_1)));
        this->_modelRenderer = new ModelRenderer(this->_engine.GetGame());
    }

    ModelManager::~ModelManager()
    {
        // models
        auto itModel = this->_models.begin();
        auto itModelEnd = this->_models.end();
        for (; itModel != itModelEnd; ++itModel)
            Tools::Delete(itModel->second);
        // model types
        auto itType = this->_modelTypes.begin();
        auto itTypeEnd = this->_modelTypes.end();
        for (; itType != itTypeEnd; ++itType)
            Tools::Delete(itType->second);
        Tools::Delete(this->_modelRenderer);
    }

    void ModelManager::Tick(Uint64 totalTime)
    {
        auto itModel = this->_models.begin();
        auto itModelEnd = this->_models.end();
        float deltaTime = float(totalTime - this->_lastTickTime) * 0.000001f;
        for (; itModel != itModelEnd; ++itModel)
            itModel->second->Update(deltaTime, 0);
        this->_lastTickTime = totalTime;
    }

    void ModelManager::Render()
    {
        auto itModel = this->_models.begin();
        auto itModelEnd = this->_models.end();
        for (; itModel != itModelEnd; ++itModel)
            this->_modelRenderer->Render(*itModel->second, itModel->second->GetDoodad().GetPosition());
    }

    void ModelManager::DeleteModelsOfDoodad(Uint32 doodadId)
    {
        auto listIt = this->_modelsByDoodad.find(doodadId);
        if (listIt == this->_modelsByDoodad.end())
            return; // le doodad n'a aucun model
        auto it = listIt->second.begin();
        auto itEnd = listIt->second.end();
        for (; it != itEnd; ++it)
        {
            this->_models.erase((*it)->GetId());
            Tools::Delete(*it);
        }
        this->_modelsByDoodad.erase(listIt);
    }

    void ModelManager::_ApiSpawn(Tools::Lua::CallHelper& helper)
    {
        Uint32 doodadId = this->_engine.GetRunningDoodadId();
        if (helper.GetNbArgs() >= 2)
            doodadId = helper.PopArg().Check<Uint32>("Client.Model.Spawn: Argument \"doodadId\" must be a number");
        std::string modelName = helper.PopArg("Client.Model.Spawn: Missing argument \"modelName\"").CheckString("Client.Model.Spawn: Argument \"modelName\" must be a string");
        if (!doodadId)
        {
            Tools::error << "ModelManager::_ApiSpawn: No doodad with id " << doodadId << ", cannot spawn model." << std::endl;
            return; // retourne nil
        }
        auto itType = this->_modelTypes.find(modelName);
        if (itType == this->_modelTypes.end())
        {
            Tools::error << "ModelManager::_ApiSpawn: No model named \"" << modelName << "\"." << std::endl;
            return; // retourne nil
        }

        // trouve le prochain modelId
        while (!this->_nextModelId // 0 est la valeur spéciale "pas de model", on la saute
                || this->_models.count(this->_nextModelId))
            ++this->_nextModelId;
        Uint32 newId = this->_nextModelId++;

        Model* m = new Model(this->_engine.GetGame().GetResourceManager(), newId, doodadId, this->_engine.GetDoodadManager().GetRunningDoodad(), itType->second);
        this->_models[newId] = m;
        this->_modelsByDoodad[doodadId].push_back(m);
        helper.PushRet(this->_engine.GetInterpreter().MakeNumber(newId));
    }

    void ModelManager::_ApiKill(Tools::Lua::CallHelper& helper)
    {
        // trouve le model
        Uint32 modelId = helper.PopArg("Client.Model.Kill: Missing argument \"modelId\"").Check<Uint32>("Client.Model.Kill: Argument \"modelId\" must be a number");
        auto it = this->_models.find(modelId);
        if (it == this->_models.end())
        {
            Tools::error << "ModelManager::_ApiKill: No model with id " << modelId << ", cannot kill model." << std::endl;
            return;
        }

        // enleve le model de la liste associée au doodad
        auto itList = this->_modelsByDoodad.find(it->second->GetDoodadId());
        assert(itList != this->_modelsByDoodad.end() && "un model de la map générale des models n'était pas dans une liste associée a un doodad");
        itList->second.remove(it->second);
        if (itList->second.empty())
            this->_modelsByDoodad.erase(itList); // supprime l'entrée associée au doodad si celui-ci n'a plus de models

        // enleve le model de la map générale + delete
        Tools::Delete(it->second);
        this->_models.erase(it);
    }

    void ModelManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        auto const& pluginName = this->_engine.GetCurrentPluginName();
        if (pluginName == "")
            throw std::runtime_error("Client.Model.Register: Could not determine currently running plugin, aborting registration");
        Tools::Lua::Ref prototype(this->_engine.GetInterpreter().GetState());
        std::string modelName;
        Uint32 resourceId;
        try
        {
            prototype = helper.PopArg("Client.Model.Register: Missing argument \"prototype\"");
            if (!prototype.IsTable())
                throw std::runtime_error("Client.Model.Register: Argument \"prototype\" must be of type table");
            if (!prototype["modelName"].IsString())
                throw std::runtime_error("Client.Model.Register: Field \"modelName\" must exist and be of type string");
            if (!Common::FieldUtils::IsRegistrableType(modelName = prototype["modelName"].ToString()))
                throw std::runtime_error("Client.Model.Register: Invalid model name \"" + modelName + "\"");
            modelName = Common::FieldUtils::GetResourceName(pluginName, modelName);
            if (this->_modelTypes.count(modelName))
                throw std::runtime_error("Client.Model.Register: A model with the name \"" + modelName + "\" is already registered");
            if (!prototype["file"].IsString())
                throw std::runtime_error("Client.Model.Register: Field \"file\" must exist and be of type string");
            resourceId = this->_engine.GetGame().GetResourceManager().GetResourceId(prototype["file"].ToString());
        }
        catch (std::exception& e)
        {
            Tools::error << "ModelManager::_ApiRegister: Failed to register new model type: " << e.what() << std::endl;
            return;
        }
        this->_modelTypes[modelName] = new ModelType(modelName, resourceId);
        Tools::debug << "Model \"" << modelName << "\" registered." << std::endl;
    }

}}}
