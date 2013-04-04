#ifndef __CLIENT_GAME_ENGINE_MODELMANAGER_HPP__
#define __CLIENT_GAME_ENGINE_MODELMANAGER_HPP__

#include <luasel/Luasel.hpp>

#include "tools/lua/AWeakResourceRef.hpp"
#include "tools/lua/WeakResourceRefManager.hpp"

namespace Tools {
    namespace Lua {
        class CallHelper;
    }
    namespace Gfx { namespace Utils {
        class DeferredShading;
    }}
}
namespace Client { namespace Game {
    class ModelRenderer;
}}

namespace Client { namespace Game { namespace Engine {

    class Engine;
    class ModelType;
    class Model;

    class ModelManager :
        private boost::noncopyable
    {
    public:
        struct WeakModelRef : public Tools::Lua::AWeakResourceRef<ModelManager>
        {
            WeakModelRef() : modelId(0) {}
            WeakModelRef(Uint32 modelId) : modelId(modelId) {}
            virtual bool IsValid(ModelManager const&) const { return this->modelId != 0; }
            virtual void Invalidate(ModelManager const&) { this->modelId = 0; }
            virtual Luasel::Ref GetReference(ModelManager& modelManager) const;
            bool operator <(WeakModelRef const& rhs) const;
            Uint32 modelId;
        };

    private:
        Engine& _engine;
        std::map<std::string /* modelName */, ModelType*> _modelTypes;
        std::map<Uint32 /* modelId */, Model*> _models;
        std::map<Uint32 /* doodadId */, std::list<Model*>> _modelsByDoodad;
        Uint32 _nextModelId;
        ModelRenderer* _modelRenderer;
        Uint64 _lastTickTime;
        Tools::Lua::WeakResourceRefManager<WeakModelRef, ModelManager>* _weakModelRefManager;

    public:
        ModelManager(Engine& engine);
        ~ModelManager();
        void Tick(Uint64 totalTime);
        void Render(Tools::Gfx::Utils::DeferredShading& deferredShading);
        void DeleteModelsOfDoodad(Uint32 doodadId);
        Luasel::Ref GetLuaWrapperForModel(Uint32 modelId);
    private:
        Model const& _GetModel(Uint32 modelId) const throw(std::runtime_error);
        Uint32 _RefToModelId(Luasel::Ref const& ref) const throw(std::runtime_error);
        void _ApiSpawn(Luasel::CallHelper& helper);
        void _ApiKill(Luasel::CallHelper& helper);
        void _ApiRegister(Luasel::CallHelper& helper);
        void _ApiBindToBody(Luasel::CallHelper& helper);
        void _ApiBindBoneToBodyNode(Luasel::CallHelper& helper);
    };

}}}

#endif
