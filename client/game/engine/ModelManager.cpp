#include <luasel/Luasel.hpp>

#include "client/game/engine/ModelManager.hpp"
#include "client/game/engine/Model.hpp"
#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/ModelType.hpp"
#include "client/game/engine/Engine.hpp"
#include "client/game/Game.hpp"
#include "client/game/ModelRenderer.hpp"
#include "tools/Math.hpp"
#include "common/FieldUtils.hpp"
#include "client/game/engine/Body.hpp"

namespace Client { namespace Game { namespace Engine {

    ModelManager::ModelManager(Engine& engine) :
        _engine(engine),
        _nextModelId(1), // le premier model sera le 1, 0 est la valeur spéciale "pas de model"
        _lastTickTime(0)
    {
        auto& i = this->_engine.GetInterpreter();

        this->_weakModelRefManager = new Tools::Lua::WeakResourceRefManager<WeakModelRef, ModelManager>(
                i, /* interpreter */
                *this, /* resource manager */
                false /* use fake references TODO recevoir ce bool depuis le reseau */);

        auto namespaceTable = i.Globals().GetTable("Client").GetTable("Model");
        namespaceTable.Set("Spawn", i.MakeFunction(std::bind(&ModelManager::_ApiSpawn, this, std::placeholders::_1)));
        namespaceTable.Set("Kill", i.MakeFunction(std::bind(&ModelManager::_ApiKill, this, std::placeholders::_1)));
        namespaceTable.Set("Register", i.MakeFunction(std::bind(&ModelManager::_ApiRegister, this, std::placeholders::_1)));
        namespaceTable.Set("BindToBody", i.MakeFunction(std::bind(&ModelManager::_ApiBindToBody, this, std::placeholders::_1)));
        namespaceTable.Set("BindBoneToBodyNode", i.MakeFunction(std::bind(&ModelManager::_ApiBindBoneToBodyNode, this, std::placeholders::_1)));
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
        // renderer
        Tools::Delete(this->_modelRenderer);
        // resource manager
        Tools::Delete(this->_weakModelRefManager);
    }

    Luasel::Ref ModelManager::WeakModelRef::GetReference(ModelManager& modelManager) const
    {
        return modelManager.GetLuaWrapperForModel(this->modelId);
    }

    bool ModelManager::WeakModelRef::operator <(WeakModelRef const& rhs) const
    {
        return this->modelId < rhs.modelId;
    }

    void ModelManager::Tick(Uint64 totalTime)
    {
        auto itModel = this->_models.begin();
        auto itModelEnd = this->_models.end();
        float deltaTime = float(totalTime - this->_lastTickTime) * 0.000001f;
        for (; itModel != itModelEnd; ++itModel)
            itModel->second->Update(deltaTime, Tools::Math::PiFloat/2.0f);
        this->_lastTickTime = totalTime;
    }

    void ModelManager::Render(Tools::Renderers::Utils::DeferredShading& deferredShading)
    {
        auto itModel = this->_models.begin();
        auto itModelEnd = this->_models.end();
        for (; itModel != itModelEnd; ++itModel)
            this->_modelRenderer->Render(deferredShading, *itModel->second, itModel->second->GetDoodad().GetPhysics(), itModel->second->GetDoodad().GetUpdateFlag());
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

    Luasel::Ref ModelManager::GetLuaWrapperForModel(Uint32 modelId)
    {
        auto& i = this->_engine.GetInterpreter();
        auto object = i.MakeTable();
        object.Set("id", modelId);
        object.Set("Kill", i.MakeFunction(std::bind(&ModelManager::_ApiKill, this, std::placeholders::_1)));
        object.Set("BindToBody", i.MakeFunction(std::bind(&ModelManager::_ApiBindToBody, this, std::placeholders::_1)));
        object.Set("BindBoneToBodyNode", i.MakeFunction(std::bind(&ModelManager::_ApiBindBoneToBodyNode, this, std::placeholders::_1)));
        return object;
    }

    Model const& ModelManager::_GetModel(Uint32 modelId) const throw(std::runtime_error)
    {
        auto it = this->_models.find(modelId);
        if (it == this->_models.end())
            throw std::runtime_error("ModelManager::_GetModel: Model " + Tools::ToString(modelId) + " not found.");
        return *it->second;
    }

    Uint32 ModelManager::_RefToModelId(Luasel::Ref const& ref) const throw(std::runtime_error)
    {
        if (ref.IsNumber()) /* id directement en nombre */
            return ref.To<Uint32>();
        else if (ref.IsTable()) /* type final model wrapper (weak pointer déréférencé) */
            return ref["id"].Check<Uint32>("ModelManager::_RefToModelId: Table argument has no id number field");
        else if (ref.IsUserData())
        {
            if (this->_weakModelRefManager->UsingFakeReferences() && ref.Is<Tools::Lua::WeakResourceRefManager<WeakModelRef, ModelManager>::FakeReference*>()) /* model wrapper en fake reference (weak pointer déréférencé) */
            {
                auto fakeRef = ref.To<Tools::Lua::WeakResourceRefManager<WeakModelRef, ModelManager>::FakeReference*>();
                if (fakeRef->IsValid())
                {
                    auto trueRef = fakeRef->GetReference();
                    if (trueRef.IsTable())
                        return trueRef["id"].Check<Uint32>("ModelManager::_RefToModelId: Table argument has no id number field");
                    else
                        throw std::runtime_error("ModelManager::_RefToModelId: Fake reference does not contain a table but a " + trueRef.GetTypeName() + ", is this really a model?");
                }
                else
                    throw std::runtime_error("ModelManager::_RefToModelId: This reference was invalidated - you must not keep true references to models, only weak references");
            }
            else /* weak pointer (non déréférencé) */
            {
                WeakModelRef* m = ref.Check<WeakModelRef*>("ModelManager::_RefToModelId: Userdata argument is not of WeakModelRef type");
                if (m->IsValid(*this))
                    return m->modelId;
                else
                    throw std::runtime_error("ModelManager::_RefToModelId: This weak pointer was invalidated - the model does not exist anymore");
            }
        }
        else
            throw std::runtime_error("ModelManager::_RefToModelId: Invalid argument type " + ref.GetTypeName() + " given");
    }

    void ModelManager::_ApiSpawn(Luasel::CallHelper& helper)
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

    void ModelManager::_ApiKill(Luasel::CallHelper& helper)
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

    void ModelManager::_ApiRegister(Luasel::CallHelper& helper)
    {
        auto const& pluginName = this->_engine.GetRunningPluginName();
        if (pluginName == "")
            throw std::runtime_error("Client.Model.Register: Could not determine currently running plugin, aborting registration");
        Luasel::Ref prototype(this->_engine.GetInterpreter().GetState());
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

    void ModelManager::_ApiBindToBody(Luasel::CallHelper& helper)
    {
        //TODO
    }

    void ModelManager::_ApiBindBoneToBodyNode(Luasel::CallHelper& helper)
    {
        // trouve le model
        Uint32 modelId = helper.PopArg("Client.Model.BindBoneToBodyNode: Missing argument \"modelId\"").Check<Uint32>("Client.Model.BindBoneToBodyNode: Argument \"modelId\" must be a number");
        std::string modelNode = helper.PopArg("Client.Model.BindBoneToBodyNode: Missing argument \"modelNode\"").Check<std::string>("Client.Model.BindBoneToBodyNode: Argument \"modelNode\" must be a number");
        std::string bodyNode = helper.PopArg("Client.Model.BindBoneToBodyNode: Missing argument \"bodyNode\"").Check<std::string>("Client.Model.BindBoneToBodyNode: Argument \"bodyNode\" must be a number");
        auto it = this->_models.find(modelId);
        if (it == this->_models.end())
        {
            Tools::error << "ModelManager::_ApiBindBoneToBodyNode: No model with id " << modelId << "\n";
            return;
        }

        auto& model = *it->second;
        auto& doodad = this->_engine.GetDoodadManager().GetDoodad(model.GetDoodadId());
        Body* body = doodad.GetBody();

        if (body == 0)
        {
            Tools::error << "Client.Model.ApiBindBoneToBodyNode: bodyless doodad\n";
            return;
        }

        // TODO la suiitititiuuiuiuite

        std::shared_ptr<Common::Physics::Node> newBone = body->BindNode(bodyNode);
        if (!newBone)
            Tools::error << "Client.Model.ApiBindBoneToBodyNode: no body node called " + bodyNode + "\n";
        else if (!model.BindBone(modelNode, newBone))
            Tools::error << "Client.Model.ApiBindBoneToBodyNode: no model bone called " + modelNode + "\n";
    }

}}}
