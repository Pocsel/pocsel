#include "client/game/ItemManager.hpp"
#include "client/game/ItemRenderer.hpp"
#include "client/game/Model.hpp"
#include "client/game/Item.hpp"
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
        for (auto it = this->_items.begin(), ite = this->_items.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    void ItemManager::MoveItem(Uint32 id, Common::MovingOrientedPosition const& pos)
    {
        if (this->_items.count(id) == 0)
        {
            std::unique_ptr<Model> model(new Model(this->_game.GetClient().GetLocalResourceManager()));
            this->_items[id] = new Item(model);
        }
        this->_items[id]->position = pos.position;
    }

    void ItemManager::Render()
    {
        for (auto it = this->_items.begin(), ite = this->_items.end(); it != ite; ++it)
        {
            this->_renderer->Render(*it->second);
        }
    }

    void ItemManager::Update(Uint32 time)
    {
        for (auto it = this->_items.begin(), ite = this->_items.end(); it != ite; ++it)
        {
            it->second->Update(time);
        }
        this->_renderer->Update(time);
    }

}}
