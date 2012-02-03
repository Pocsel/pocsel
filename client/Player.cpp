#include "client/Player.hpp"
#include "client/PacketDispatcher.hpp"
#include "client/PacketExtractor.hpp"

#include "tools/gui/EventManager.hpp"
#include "tools/gui/EventCaster.hpp"
#include "tools/gui/events/BindAction.hpp"

namespace Client {

    Player::Player(Tools::Gui::EventManager& eventManager, PacketDispatcher& packetDispatcher) :
        _eventManager(eventManager)
    {
        packetDispatcher.Register(
            Protocol::ServerToClient::SpawnPosition,
            std::bind(&Player::_HandleSpawnPosition, this, std::placeholders::_1)
        );

        this->_eventManager.Connect("bind/forward", std::bind(&Player::MoveForward, this));
        this->_eventManager.Connect("bind/backward", std::bind(&Player::MoveBackward, this));
        this->_eventManager.Connect("bind/left", std::bind(&Player::StrafeLeft, this));
        this->_eventManager.Connect("bind/right", std::bind(&Player::StrafeRight, this));
        this->_eventManager.Connect(
            "bind/jump",
            Tools::Gui::EventCaster<Tools::Gui::Events::BindAction, Player>(&Player::_OnBindJump, this)
        );
    }

    void Player::SetPosition(Common::Position const& pos)
    {
        if (pos.world != this->_camera.position.world)
            this->_eventManager.Notify<Tools::Gui::Event>("player/changechunk");
        this->_camera.position = pos;
        this->_eventManager.Notify<Tools::Gui::Event>("player/moved");
    }

    void Player::MoveForward()
    {
        this->_Move(this->_camera.direction);
    }

    void Player::MoveBackward()
    {
        this->_Move(-this->_camera.direction);
    }

    void Player::StrafeLeft()
    {
        Tools::Vector3f dir = this->_camera.direction;
        this->_Move(Tools::Vector3f(
            dir.y * 0 - dir.z * -1,
            dir.z * 0 - dir.x * 0,
            dir.x * -1 - dir.y * 0
        ));
    }

    void Player::StrafeRight()
    {
        Tools::Vector3f dir = this->_camera.direction;
        this->_Move(Tools::Vector3f(
            dir.y * 0 - dir.z * 1,
            dir.z * 0 - dir.x * 0,
            dir.x * 1 - dir.y * 0
        ));
    }

    void Player::Jump()
    {
        this->_Move(Tools::Vector3f(0, 1, 0));
    }

    void Player::_OnBindJump(Tools::Gui::Events::BindAction const& e)
    {
        if (e.type == Tools::Gui::Events::BindAction::Type::Pressed)
            this->Jump();
    }

    void Player::_Move(Tools::Vector3f moveVector)
    {
        moveVector.Normalize();
        Common::Position p = this->_camera.position;
        p += moveVector;
        this->SetPosition(p);
    }

    void Player::_HandleSpawnPosition(Common::Packet const& p)
    {
        Common::Position pos;
        PacketExtractor::ExtractSpawnPosition(p, pos);
        this->SetPosition(pos);
    }

}
