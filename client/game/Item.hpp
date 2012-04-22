#ifndef __CLIENT_GAME_ITEM_HPP__
#define __CLIENT_GAME_ITEM_HPP__

#include "client/game/Model.hpp"

#include "common/OrientedPosition.hpp"

namespace Client { namespace Game {

    class Item
    {
    public:
        Common::OrientedPosition position;
    private:
        std::unique_ptr<Model> _model;

    public:
        Item(std::unique_ptr<Model>& model);

        void Update(Uint32 time);

        Model const& GetModel() const { return *this->_model; }
    };

}}

#endif

