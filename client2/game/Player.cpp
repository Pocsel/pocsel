#include "client2/precompiled.hpp"

#include "client2/game/Player.hpp"
#include "client2/network/PacketDispatcher.hpp"
#include "client2/window/InputManager.hpp"
#include "client2/window/Window.hpp"

namespace Client { namespace Game {

    Player::Player()
    {
    }

    void Player::UpdateMovements(Window::Window& window)
    {
        if (!window.GetInputManager().HasFocus())
            return;

        if (window.GetInputManager().GetActionState(BindAction::Forward) != BindAction::Released)
            this->MoveForward();
        if (window.GetInputManager().GetActionState(BindAction::Backward) != BindAction::Released)
            this->MoveBackward();
        if (window.GetInputManager().GetActionState(BindAction::Left) != BindAction::Released)
            this->StrafeLeft();
        if (window.GetInputManager().GetActionState(BindAction::Right) != BindAction::Released)
            this->StrafeRight();

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

    void Player::_Move(Tools::Vector3f moveVector)
    {
        moveVector.Normalize();
        Common::Position p = this->_camera.position;
        p += moveVector;
        this->SetPosition(p);
    }

}}