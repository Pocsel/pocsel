#ifndef __CLIENT_GAME_HPP__
#define __CLIENT_GAME_HPP__

#include <boost/noncopyable.hpp>

#include "tools/IRenderer.hpp"
#include "tools/gui/events/all_forward.hpp"

#include "client/Player.hpp"
#include "client/World.hpp"
#include "client/CubeTypeManager.hpp"

namespace Common {
    class Packet;
}

namespace Client
{

    class Application;

    class Game :
        private boost::noncopyable
    {
        private:
            Application& _app;
            Player _player;
            World _world;
            CubeTypeManager _cubeTypeManager;
            CacheDatabaseProxy* _database;
            ResourceManager* _resourceManager;
            bool _isStarted;
            bool _cacheLoaded;
            bool _cubeTypesLoaded;

        public:
            Game(Application& app);
            ~Game();
            Player& GetPlayer() { return this->_player; }
            World& GetWorld() { return this->_world; }
            CacheDatabaseProxy& GetCacheDatabaseProxy() { return *this->_database; }
            ResourceManager& GetResourceManager() { return *this->_resourceManager; }
            CubeTypeManager& GetCubeTypeManager() { return this->_cubeTypeManager; }

            void Render(Tools::IRenderer& renderer);

        private:
            void _Start();
            void _TryStart();
            void _CacheLoaded();
            void _CubeTypesLoaded();
            void _HandleChunk(Common::Packet& p);
            void _AskForChunks();
            void _OnResizeWidget(Tools::Gui::Events::Resize const& evt);
    };

}

#endif
