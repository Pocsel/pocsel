#include "client/game/ItemManager.hpp"
#include "client/game/ItemRenderer.hpp"
#include "client/game/Model.hpp"
#include "client/game/Game.hpp"

#include "common/MovingOrientedPosition.hpp"

#include "client/Client.hpp"

#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    ItemManager::ItemManager(Game& game) :
        _game(game),
        _renderer(new ItemRenderer(game))
    {
    }

    ItemManager::~ItemManager()
    {
        Tools::Delete(this->_renderer);
    }

    void ItemManager::MoveItem(Uint32 id, Common::MovingOrientedPosition const& pos)
    {
        if (this->_positions.count(id) == 0)
        {
            this->_positions[id].second =
                new Model(
                        this->_game.GetClient().GetLocalResourceManager().GetMd5Model("boblampclean")
                        );
        }
        this->_positions[id].first = pos.position;
    }

    void ItemManager::Render()
    {
        for (auto it = this->_positions.begin(), ite = this->_positions.end(); it != ite; ++it)
        {
            this->_renderer->Render(*it->second.second, it->second.first);
        }
    }

    void ItemManager::Update(Uint32 time)
    {
        for (auto it = this->_positions.begin(), ite = this->_positions.end(); it != ite; ++it)
        {
            it->second.second->Update(time, it->second.first.phi);
        }
        this->_renderer->Update(time);
    }

}}
