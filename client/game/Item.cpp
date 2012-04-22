#include "client/game/Item.hpp"

#include "common/OrientedPosition.hpp"

namespace Client { namespace Game {

    Item::Item(std::unique_ptr<Model>& model) :
        _model(model.release())
    {
    }

    void Item::Update(Uint32 time)
    {
        this->_model->Update(time, this->position.phi);
    }

}}
