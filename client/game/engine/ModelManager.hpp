#ifndef __CLIENT_GAME_ENGINE_MODELMANAGER_HPP__
#define __CLIENT_GAME_ENGINE_MODELMANAGER_HPP__

namespace Tools { namespace Lua {
    class CallHelper;
}}
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
    private:
        Engine& _engine;
        std::map<std::string /* modelName */, ModelType*> _modelTypes;
        std::map<Uint32 /* modelId */, Model*> _models;
        std::map<Uint32 /* doodadId */, std::list<Model*>> _modelsByDoodad;
        Uint32 _nextModelId;
        ModelRenderer* _modelRenderer;
        Uint64 _lastTickTime;

    public:
        ModelManager(Engine& engine);
        ~ModelManager();
        void Tick(Uint64 totalTime);
        void Render();
        void DeleteModelsOfDoodad(Uint32 doodadId);
    private:
        void _ApiSpawn(Tools::Lua::CallHelper& helper);
        void _ApiKill(Tools::Lua::CallHelper& helper);
        void _ApiRegister(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
