#include "client/game/ItemManager.hpp"
#include "client/game/ItemRenderer.hpp"

#include "common/MovingOrientedPosition.hpp"

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
        this->_positions[id] = pos.position;
    }

    void ItemManager::Render()
    {
        for (auto it = this->_positions.begin(), ite = this->_positions.end(); it != ite; ++it)
        {
            this->_renderer->Render(it->second);
        }
    }

    void ItemManager::Update(Uint32 time)
    {
        this->_renderer->Update(time);
    }

}}
