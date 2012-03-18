#include "client/game/ItemManager.hpp"
#include "client/game/ItemRenderer.hpp"

#include "common/MovingOrientedPosition.hpp"

namespace Client { namespace Game {

    ItemManager::ItemManager(Game& game) :
        _game(game),
        _pos(0),
        _renderer(new ItemRenderer(game))
    {
    }

    ItemManager::~ItemManager()
    {
        Tools::Delete(this->_pos);
        Tools::Delete(this->_renderer);
    }

    void ItemManager::MoveItem(Uint32 id, Common::MovingOrientedPosition const& pos)
    {
        if (this->_pos == 0)
            this->_pos = new Common::OrientedPosition(pos.position);

        this->_pos->SetAngles(pos.position.theta, pos.position.phi);
    }

    void ItemManager::Render()
    {
        if (_pos == 0)
            return;

        this->_renderer->Render(*this->_pos);
    }

    void ItemManager::Update(Uint32 time)
    {
        this->_renderer->Update(time);
    }

}}
