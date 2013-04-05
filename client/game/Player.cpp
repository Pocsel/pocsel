#include "client/precompiled.hpp"

#include "tools/window/InputManager.hpp"
#include "tools/window/Window.hpp"
#include "tools/sound/ISoundSystem.hpp"

#include "common/RayCast.hpp"
#include "common/MovingOrientedPosition.hpp"

#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/game/TargetedCubeRenderer.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/Map.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/network/PacketDispatcher.hpp"
#include "client/Client.hpp"
#include "client/Settings.hpp"

namespace Client { namespace Game {

    Player::Player(Game& game) :
        _game(game),
        _moved(false),
        _movedTime(0),
        _sprint(false),
        _targetedCube(0),
        _targetedCubeRenderer(new TargetedCubeRenderer(game))
    {
        this->_actionBinder.Bind(Tools::Window::BindAction::Forward, Tools::Window::BindAction::Held, std::bind(&Player::MoveForward, this));
        this->_actionBinder.Bind(Tools::Window::BindAction::Backward, Tools::Window::BindAction::Held, std::bind(&Player::MoveBackward, this));
        this->_actionBinder.Bind(Tools::Window::BindAction::Left, Tools::Window::BindAction::Held, std::bind(&Player::StrafeLeft, this));
        this->_actionBinder.Bind(Tools::Window::BindAction::Right, Tools::Window::BindAction::Held, std::bind(&Player::StrafeRight, this));

        this->_actionBinder.Bind(Tools::Window::BindAction::Jump, Tools::Window::BindAction::Held, std::bind(&Player::Jump, this));
        this->_actionBinder.Bind(Tools::Window::BindAction::Crouch, Tools::Window::BindAction::Held, std::bind(&Player::Crouch, this));

        this->_actionBinder.Bind(Tools::Window::BindAction::Fire, Tools::Window::BindAction::Pressed, std::bind(&Player::Action, this));
        //this->_actionBinder.Bind(BindAction::Fire, BindAction::Held, std::bind(&Player::Action, this));

        this->_actionBinder.Bind(Tools::Window::BindAction::AltFire, Tools::Window::BindAction::Pressed, std::bind(&Player::SuperAction, this));
        //this->_actionBinder.Bind(BindAction::AltFire, BindAction::Held, std::bind(&Player::SuperAction, this));

        this->_actionBinder.Bind(Tools::Window::BindAction::ToggleSprint, Tools::Window::BindAction::Pressed, std::bind(&Player::ToggleSprint, this));
    }

    Player::~Player()
    {
        Tools::Delete(this->_targetedCubeRenderer);
        Tools::Delete(this->_targetedCube);
    }

    void Player::Render()
    {
        if (this->_targetedCube != 0)
            this->_targetedCubeRenderer->Render(*this->_targetedCube);
    }

    void Player::UpdateMovements(Uint64 deltaTime)
    {
        this->_elapsedTime = float(deltaTime) * 0.000001f;

        auto& w = this->_game.GetClient().GetWindow();
        this->_actionBinder.Dispatch(w.GetInputManager());

        if (w.GetInputManager().HasFocus() && !w.GetInputManager().MouseShown())
        {
            auto sensi = this->_game.GetClient().GetSettings().mouseSensitivity;
            auto mousePos = w.GetInputManager().GetMousePosRealTime();
            glm::fvec2 delta;
            delta.x = (mousePos.x - (static_cast<int>(w.GetSize().x) / 2)) / (1000.0f - 990.0f * sensi);
            delta.y = (mousePos.y - (static_cast<int>(w.GetSize().y) / 2)) / (1000.0f - 990.0f * sensi);
            if (delta.x != 0.0f || delta.y != 0.0f)
            {
                this->_camera.Rotate(delta);
                this->_moved = true;
            }
            w.GetInputManager().WarpMouse(glm::ivec2(w.GetSize() / (std::remove_reference<decltype(w.GetSize())>::type::value_type)2));
        }

        if (this->_movement != glm::fvec3())
            this->_Move();

        this->_movedTime += (Uint32)deltaTime;
        if (this->_moved && this->_movedTime > 40000)
        {
            this->_game.GetClient().GetNetwork().SendUdpPacket(
                Network::PacketCreator::Move(this->_game.GetClient().GetClientId(), Common::MovingOrientedPosition(this->_camera, this->_movement * this->_GetSpeed()))
                );
            this->_moved = false;
            this->_movedTime %= 40000;
        }

        this->_movement = glm::fvec3();

        {
            auto cubes = Common::RayCast::Ray(this->_camera, 50);

            Common::CubePosition cubePos;

            if (!this->_game.GetMap().GetFirstCube(cubes, cubePos))
            {
                Tools::Delete(this->_targetedCube);
                this->_targetedCube = 0;
            }
            else
            {
                if (this->_targetedCube != 0)
                    *this->_targetedCube = cubePos;
                else
                    this->_targetedCube = new Common::CubePosition(cubePos);
            }
        }

        this->_targetedCubeRenderer->Update(deltaTime);
    }

    void Player::SetPosition(Common::Position const& pos)
    {
        this->_camera.position = pos;
        this->_game.GetSoundSystem().SetEars(glm::fvec3(pos));
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
        glm::fvec3 dir = this->_camera.direction;
        this->_Move(glm::fvec3(
            dir.y * 0 - dir.z * -1,
            dir.z * 0 - dir.x * 0,
            dir.x * -1 - dir.y * 0));
    }

    void Player::StrafeRight()
    {
        glm::fvec3 dir = this->_camera.direction;
        this->_Move(glm::fvec3(
            dir.y * 0 - dir.z * 1,
            dir.z * 0 - dir.x * 0,
            dir.x * 1 - dir.y * 0));
    }

    void Player::Jump()
    {
        this->_Move(glm::fvec3(0, 1, 0));
    }

    void Player::Crouch()
    {
        this->_Move(glm::fvec3(0, -1, 0));
    }

    void Player::Action()
    {
        Common::CubePosition target;
        if (this->_targetedCube != 0)
            target = *this->_targetedCube;
        else
            target = Common::CubePosition(this->_camera.position);

        this->_game.GetClient().GetNetwork().SendUdpPacket(
            Network::PacketCreator::Action(
                this->_game.GetClient().GetClientId(),
                this->_camera,
                target,
                1
                )
            );
    }

    void Player::SuperAction()
    {
        Common::CubePosition target;
        if (this->_targetedCube != 0)
            target = *this->_targetedCube;
        else
            target = Common::CubePosition(this->_camera.position);

        this->_game.GetClient().GetNetwork().SendUdpPacket(
            Network::PacketCreator::Action(
                this->_game.GetClient().GetClientId(),
                this->_camera,
                target,
                2
                )
            );
    }

    void Player::ToggleSprint()
    {
        this->_sprint = !this->_sprint;
    }

    void Player::_Move(glm::fvec3 moveVector)
    {
        moveVector = glm::normalize(moveVector);
        this->_movement += moveVector;
    }

    void Player::_Move()
    {
        this->_movement = glm::normalize(this->_movement);
        auto tmp = glm::dvec3(this->_movement) * (double)(this->_GetSpeed() * this->_elapsedTime);
        Common::Position p = this->_camera.position;
        p += tmp;
        this->SetPosition(p);
        this->_moved = true;
    }

    float Player::_GetSpeed()
    {
        return this->_sprint ? 133.3333f : 13.333333f;
    }

}}
