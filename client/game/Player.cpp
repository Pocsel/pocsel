#include "client/precompiled.hpp"

#include "client/game/Player.hpp"
#include "client/network/PacketDispatcher.hpp"
#include "client/window/InputManager.hpp"
#include "client/window/Window.hpp"

namespace Client { namespace Game {

    Player::Player()
    {
        this->_actionBinder.Bind(BindAction::Forward, BindAction::Held, std::bind(&Player::MoveForward, this));
        this->_actionBinder.Bind(BindAction::Backward, BindAction::Held, std::bind(&Player::MoveBackward, this));
        this->_actionBinder.Bind(BindAction::Left, BindAction::Held, std::bind(&Player::StrafeLeft, this));
        this->_actionBinder.Bind(BindAction::Right, BindAction::Held, std::bind(&Player::StrafeRight, this));
    }

    void Player::UpdateMovements(Window::Window& window, Uint32 time)
    {
        if (!window.GetInputManager().HasFocus())
            return;

        this->_elapsedTime = time;

        this->_actionBinder.Dispatch(window.GetInputManager());

        auto delta = (Tools::Vector2f(window.GetInputManager().GetMousePos()) - (Tools::Vector2f(window.GetSize()) / 2.0f)) / 300.0f;
        this->_camera.Rotate(delta);
        window.GetInputManager().WarpMouse(Tools::Vector2i(window.GetSize() / 2));
    }

    void Player::SetPosition(Common::Position const& pos)
    {
        this->_camera.position = pos;
    }

    void Player::MoveForward()
    {
        this->_Move(this->_camera.direction * (this->_elapsedTime / 100.0f));
    }

    void Player::MoveBackward()
    {
        this->_Move(-this->_camera.direction * (this->_elapsedTime / 100.0f));
    }

    void Player::StrafeLeft()
    {
        Tools::Vector3f dir = this->_camera.direction;
        this->_Move(Tools::Vector3f(
            dir.y * 0 - dir.z * -1,
            dir.z * 0 - dir.x * 0,
            dir.x * -1 - dir.y * 0
        ) * (this->_elapsedTime / 100.0f));
    }

    void Player::StrafeRight()
    {
        Tools::Vector3f dir = this->_camera.direction;
        this->_Move(Tools::Vector3f(
            dir.y * 0 - dir.z * 1,
            dir.z * 0 - dir.x * 0,
            dir.x * 1 - dir.y * 0
        ) * (this->_elapsedTime / 100.0f));
    }

    void Player::Jump()
    {
        this->_Move(Tools::Vector3f(0, 1, 0));
    }

    void Player::_Move(Tools::Vector3f moveVector)
    {
        moveVector.Normalize();
        Common::Position p = this->_camera.position;
        p += moveVector;
        this->SetPosition(p);
    }

}}
