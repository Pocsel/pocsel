#include "client/precompiled.hpp"

#include "client/game/Player.hpp"
#include "client/network/PacketDispatcher.hpp"
#include "client/window/InputManager.hpp"
#include "client/window/Window.hpp"
#include "client/game/Game.hpp"
#include "client/Client.hpp"
#include "client/Settings.hpp"

namespace Client { namespace Game {

    Player::Player(Game& game) :
        _game(game)
    {
        this->_actionBinder.Bind(BindAction::Forward, BindAction::Held, std::bind(&Player::MoveForward, this));
        this->_actionBinder.Bind(BindAction::Backward, BindAction::Held, std::bind(&Player::MoveBackward, this));
        this->_actionBinder.Bind(BindAction::Left, BindAction::Held, std::bind(&Player::StrafeLeft, this));
        this->_actionBinder.Bind(BindAction::Right, BindAction::Held, std::bind(&Player::StrafeRight, this));
    }

    void Player::UpdateMovements(Uint32 time)
    {
        this->_elapsedTime = time;

        Window::Window& w = this->_game.GetClient().GetWindow();
        this->_actionBinder.Dispatch(w.GetInputManager());

        if (w.GetInputManager().HasFocus())
        {
            Tools::Vector2f delta;
            delta.x = (w.GetInputManager().GetMousePos().x - (static_cast<int>(w.GetSize().x) / 2)) / (1000.0f - 990.0f * this->_game.GetClient().GetSettings().mouseSensitivity);
            delta.y = (w.GetInputManager().GetMousePos().y - (static_cast<int>(w.GetSize().y) / 2)) / (1000.0f - 990.0f * this->_game.GetClient().GetSettings().mouseSensitivity);
            this->_camera.Rotate(delta);
            w.GetInputManager().WarpMouse(Tools::Vector2i(w.GetSize() / 2));
        }
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
