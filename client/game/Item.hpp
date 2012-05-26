#ifndef __CLIENT_GAME_ITEM_HPP__
#define __CLIENT_GAME_ITEM_HPP__

#include "client/game/engine/Model.hpp"

#include "common/OrientedPosition.hpp"

namespace Client { namespace Game {

    class Item
    {
    public:
        Common::OrientedPosition position;
    private:
        std::unique_ptr<Engine::Model> _model;
        bool _moving;

    public:
        Item(std::unique_ptr<Engine::Model>& model);

        void Update(Uint32 time);

        Engine::Model const& GetModel() const { return *this->_model; }
        void SetMoving(bool moving)
        {
            if (this->_moving != moving)
            {
                this->_moving = moving;
                this->_model->SetAnim(moving ? "move" : "idle");
            }
        }
    };

}}

#endif

