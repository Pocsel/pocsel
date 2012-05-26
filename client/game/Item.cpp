#include "client/game/Item.hpp"

#include "common/OrientedPosition.hpp"

namespace Client { namespace Game {

    Item::Item(std::unique_ptr<Engine::Model>& model) :
        _model(model.release()),
        _moving(false)
    {
    }

    void Item::Update(Uint32 time)
    {
        this->_model->Update(time, this->position.phi);
    }

}}
