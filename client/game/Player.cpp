#include "client/precompiled.hpp"

#include "client/game/Player.hpp"
#include "client/network/PacketDispatcher.hpp"
#include "client/window/InputManager.hpp"
#include "client/window/Window.hpp"

namespace Client { namespace Game {

    Player::Player()
    {
    }

    void Player::UpdateMovements(Window::Window& window, Uint32 time)
    {
        if (!window.GetInputManager().HasFocus())
            return;

        if (window.GetInputManager().GetActionState(BindAction::Forward) != BindAction::Released)
            this->MoveForward(time);
        if (window.GetInputManager().GetActionState(BindAction::Backward) != BindAction::Released)
            this->MoveBackward(time);
        if (window.GetInputManager().GetActionState(BindAction::Left) != BindAction::Released)
            this->StrafeLeft(time);
        if (window.GetInputManager().GetActionState(BindAction::Right) != BindAction::Released)
            this->StrafeRight(time);

        auto delta = (Tools::Vector2f(window.GetInputManager().GetMousePos()) - (Tools::Vector2f(window.GetSize()) / 2.0f)) / 300.0f;
        this->_camera.Rotate(delta);
        window.GetInputManager().WarpMouse(Tools::Vector2i(window.GetSize() / 2));
    }

    void Player::SetPosition(Common::Position const& pos)
    {
        this->_camera.position = pos;
    }

    void Player::MoveForward(Uint32 time)
    {
        this->_Move(this->_camera.direction * (time / 100.0f));
    }

    void Player::MoveBackward(Uint32 time)
    {
        this->_Move(-this->_camera.direction * (time / 100.0f));
    }

    void Player::StrafeLeft(Uint32 time)
    {
        Tools::Vector3f dir = this->_camera.direction;
        this->_Move(Tools::Vector3f(
            dir.y * 0 - dir.z * -1,
            dir.z * 0 - dir.x * 0,
            dir.x * -1 - dir.y * 0
        ) * (time / 100.0f));
    }

    void Player::StrafeRight(Uint32 time)
    {
        Tools::Vector3f dir = this->_camera.direction;
        this->_Move(Tools::Vector3f(
            dir.y * 0 - dir.z * 1,
            dir.z * 0 - dir.x * 0,
            dir.x * 1 - dir.y * 0
        ) * (time / 100.0f));
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
