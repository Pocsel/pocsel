#include <iostream>

#include "client/Application.hpp"
#include "client/Game.hpp"
#include "client/Chunk.hpp"
#include "client/PacketCreator.hpp"
#include "client/PacketExtractor.hpp"
#include "client/CacheDatabaseProxy.hpp"
#include "client/ResourceManager.hpp"

#include "common/Packet.hpp"
#include "common/Position.hpp"

#include "tools/gui/events/Boolean.hpp"
#include "tools/gui/events/Resize.hpp"
#include "tools/gui/EventCaster.hpp"
#include "tools/gui/Window.hpp"

namespace Client {

    Game::Game(Application& app) :
        _app(app),
        _player(app.GetEventManager(), app.GetPacketDispatcher()),
        _world(app.GetEventManager(),
                app.GetNetwork(),
                app.GetWindow().GetRenderer(),
                app.GetPacketDispatcher(),
                _player),
        _cubeTypeManager(app.GetEventManager(),
                         app.GetNetwork(),
                         app.GetPacketDispatcher()),
        _isStarted(false),
        _cacheLoaded(false),
        _cubeTypesLoaded(false)
    {
        this->_database = new CacheDatabaseProxy(this->_app.GetNetwork(), this->_app.GetEventManager());
        this->_resourceManager = new ResourceManager(this->_app.GetEventManager(),
                this->_app.GetNetwork(),
                this->_app.GetWindow().GetRenderer(),
                this->_app.GetPacketDispatcher(),
                *this->_database);

        this->_app.GetEventManager().Connect(
                "cache/loaded",
                std::bind(&Game::_CacheLoaded, this)
                );

        this->_app.GetEventManager().Connect(
                "cubetypes/loaded",
                std::bind(&Game::_CubeTypesLoaded, this)
                );


        this->_app.GetEventManager().Connect(
                "player/changechunk",
                std::bind(&Game::_AskForChunks, this)
                );

        this->_app.GetEventManager().Connect(
            "gamewidget/resize",
            Tools::Gui::EventCaster<Tools::Gui::Events::Resize, Game>(&Game::_OnResizeWidget, this));


        this->_app.GetEventManager().Notify<Tools::Gui::Events::Boolean>("window/cursor-show", false);
        this->_app.GetEventManager().Notify<Tools::Gui::Events::Boolean>("window/cursor-auto-center", true);

        app.GetPacketDispatcher().Register(
                Protocol::ServerToClient::Chunk,
                std::bind(&Game::_HandleChunk, this, std::placeholders::_1)
                );
    }

    Game::~Game()
    {
        Tools::Delete(this->_resourceManager);
        Tools::Delete(this->_database);
    }

    void Game::Render(Tools::IRenderer& renderer)
    {
        renderer.SetProjectionMatrix(this->GetPlayer().GetCamera().projection);
        renderer.SetViewMatrix(this->GetPlayer().GetCamera().GetViewMatrix());
        if (this->_isStarted)
            this->GetWorld().RenderChunks();
    }

    void Game::_Start()
    {
        this->_player.SetNick(this->_app.GetUser().GetLogin());
        {
            Common::Position p;
            p.world.x = 1 << 21;
            p.world.y = (1 << 19) + 30;
            p.world.z = 1 << 21;
            p.chunk.x = 0;
            p.chunk.y = 0;
            p.chunk.z = 0;
            this->_player.SetPosition(p);
            this->_player.GetCamera().SetAngles(0, std::atan2(0.0f, -1.0f) / 2);
        }

        auto cubeTypes = this->_cubeTypeManager.GetCubeTypes();
        assert(cubeTypes.size() != 0);
        auto texturesPtr = std::unique_ptr<std::vector<Tools::Renderers::ITexture2D*>>(new std::vector<Tools::Renderers::ITexture2D*>());
        auto cubeInfosPtr = std::unique_ptr<std::vector<Client::Gui::Primitives::Chunk::CubeInfo>>(new std::vector<Client::Gui::Primitives::Chunk::CubeInfo>());
        {
            // Add air
            auto cubeInfo = Client::Gui::Primitives::Chunk::CubeInfo();
            cubeInfo.textureIndex = 0;
            cubeInfo.bottom = 0;
            cubeInfo.side = 0;
            cubeInfo.top = 0;
            cubeInfo.height = 1;
            cubeInfo.isTransparent = true;
            cubeInfosPtr->push_back(cubeInfo);
        }
        for (auto it = cubeTypes.begin(), ite = cubeTypes.end() ; it != ite ; ++it)
        {

            std::cout << it->id << std::endl;
            std::cout << it->name << std::endl;
            std::cout << it->textures.top << std::endl;
            std::cout << it->textures.left << std::endl;
            std::cout << it->textures.front << std::endl;
            std::cout << it->textures.right << std::endl;
            std::cout << it->textures.back << std::endl;
            std::cout << it->textures.bottom << std::endl;

            // TODO textures des cotes aussi :(
            auto texture = this->_resourceManager->GetTexture2D(it->textures.top);
            assert(texture != 0 && "Le resource manager cree une fake texture en theorie :(");
            if (texturesPtr->size() < it->textures.top + 1)
                texturesPtr->resize(it->textures.top + 1);
            (*texturesPtr)[it->textures.top] = texture;

            auto cubeInfo = Client::Gui::Primitives::Chunk::CubeInfo();
            cubeInfo.textureIndex = it->textures.top;
            cubeInfo.bottom = 0;
            cubeInfo.side = 0;
            cubeInfo.top = 0;
            cubeInfo.height = 1;
            cubeInfo.isTransparent = texture->HasAlpha();
            cubeInfosPtr->push_back(cubeInfo);
        }
        Client::Gui::Primitives::Chunk::Init(this->_app.GetWindow().GetRenderer(), texturesPtr, cubeInfosPtr);

        this->_AskForChunks();
        this->_isStarted = true;

        {
            auto p = PacketCreator::GetSpawnPositionPacket();
            this->_app.GetNetwork().SendPacket(std::move(p));
        }
    }

    void Game::_TryStart()
    {
        if (this->_cacheLoaded == true &&
            this->_cubeTypesLoaded == true)
            this->_Start();
    }

    void Game::_CacheLoaded()
    {
        this->_cacheLoaded = true;
        this->_TryStart();
    }

    void Game::_CubeTypesLoaded()
    {
        this->_cubeTypesLoaded = true;
        this->_TryStart();
    }

    void Game::_HandleChunk(Common::Packet& p)
    {
        std::unique_ptr<Chunk> newChunk = PacketExtractor::ExtractChunk(p);
        this->_world.InsertChunk(newChunk);
    }

    void Game::_AskForChunks()
    {
        std::deque<Chunk::IdType> neededChunks = this->_world.GetNeededChunks();
        while (neededChunks.size())
        {
            auto p = PacketCreator::NeedChunksPacket(neededChunks);
            this->_app.GetNetwork().SendPacket(std::move(p));
        }
    }

    void Game::_OnResizeWidget(Tools::Gui::Events::Resize const& evt)
    {
        this->GetPlayer().GetCamera().projection = Tools::Matrix4<float>::CreatePerspective(90, evt.width / float(evt.height), 0.001f, 1000.0f);
    }

}
