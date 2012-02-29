#include "client/precompiled.hpp"

#include "client/game/Player.hpp"
#include "client/game/TargetedCubeRenderer.hpp"
#include "client/network/PacketDispatcher.hpp"
#include "client/window/InputManager.hpp"
#include "client/window/Window.hpp"
#include "client/game/Game.hpp"
#include "client/Client.hpp"
#include "client/Settings.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/Map.hpp"

#include "client/network/PacketCreator.hpp"
#include "client/network/Network.hpp"

#include "common/RayCast.hpp"
#include "common/CubePosition.hpp"

namespace Client { namespace Game {

    Player::Player(Game& game) :
        _game(game),
        _moved(false),
        _movedTime(0),
        _sprint(false),
        _targetedCube(0),
        _targetedCubeRenderer(new TargetedCubeRenderer(game))
    {
        this->_actionBinder.Bind(BindAction::Forward, BindAction::Held, std::bind(&Player::MoveForward, this));
        this->_actionBinder.Bind(BindAction::Backward, BindAction::Held, std::bind(&Player::MoveBackward, this));
        this->_actionBinder.Bind(BindAction::Left, BindAction::Held, std::bind(&Player::StrafeLeft, this));
        this->_actionBinder.Bind(BindAction::Right, BindAction::Held, std::bind(&Player::StrafeRight, this));

        this->_actionBinder.Bind(BindAction::Jump, BindAction::Held, std::bind(&Player::Jump, this));
        this->_actionBinder.Bind(BindAction::Crouch, BindAction::Held, std::bind(&Player::Crouch, this));

        this->_actionBinder.Bind(BindAction::Fire, BindAction::Pressed, std::bind(&Player::Action, this));
        //this->_actionBinder.Bind(BindAction::Fire, BindAction::Held, std::bind(&Player::Action, this));

        this->_actionBinder.Bind(BindAction::AltFire, BindAction::Pressed, std::bind(&Player::SuperAction, this));
        //this->_actionBinder.Bind(BindAction::AltFire, BindAction::Held, std::bind(&Player::SuperAction, this));

        this->_actionBinder.Bind(BindAction::ToggleSprint, BindAction::Pressed, std::bind(&Player::ToggleSprint, this));
    }

    Player::~Player()
    {
        Tools::Delete(this->_targetedCubeRenderer);
    }

    void Player::Render()
    {
        if (this->_targetedCube != 0)
            this->_targetedCubeRenderer->Render(*this->_targetedCube);
    }

    void Player::UpdateMovements(Uint32 time)
    {
        this->_elapsedTime = time;

        auto& w = this->_game.GetClient().GetWindow();
        this->_actionBinder.Dispatch(w.GetInputManager());

        if (w.GetInputManager().HasFocus() && !w.GetInputManager().MouseShown())
        {
            auto sensi = this->_game.GetClient().GetSettings().mouseSensitivity;
            auto mousePos = w.GetInputManager().GetMousePosRealTime();
            Tools::Vector2f delta;
            delta.x = (mousePos.x - (static_cast<int>(w.GetSize().x) / 2)) / (1000.0f - 990.0f * sensi);
            delta.y = (mousePos.y - (static_cast<int>(w.GetSize().y) / 2)) / (1000.0f - 990.0f * sensi);
            if (delta.x != 0.0f || delta.y != 0.0f)
            {
                this->_camera.Rotate(delta);
                this->_moved = true;
            }
            w.GetInputManager().WarpMouse(Tools::Vector2i(w.GetSize() / 2));
        }

        this->_movedTime += time;
        if (this->_moved && this->_movedTime > 40)
        {
            this->_game.GetClient().GetNetwork().SendUdpPacket(
                Network::PacketCreator::Move(this->_game.GetClient().GetClientId(), this->_camera)
                );
            this->_moved = false;
            this->_movedTime %= 40;
        }

        {
            auto cubes = Common::RayCast::GetResult(this->_camera, 50);

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
    }

    void Player::SetPosition(Common::Position const& pos)
    {
        this->_camera.position = pos;
    }

    void Player::MoveForward()
    {
        this->_Move(this->_camera.direction * this->_GetSpeed());
    }

    void Player::MoveBackward()
    {
        this->_Move(-this->_camera.direction * this->_GetSpeed());
    }

    void Player::StrafeLeft()
    {
        Tools::Vector3f dir = this->_camera.direction;
        this->_Move(Tools::Vector3f(
            dir.y * 0 - dir.z * -1,
            dir.z * 0 - dir.x * 0,
            dir.x * -1 - dir.y * 0));
    }

    void Player::StrafeRight()
    {
        Tools::Vector3f dir = this->_camera.direction;
        this->_Move(Tools::Vector3f(
            dir.y * 0 - dir.z * 1,
            dir.z * 0 - dir.x * 0,
            dir.x * 1 - dir.y * 0));
    }

    void Player::Jump()
    {
        this->_Move(Tools::Vector3f(0, 1, 0));
    }

    void Player::Crouch()
    {
        this->_Move(Tools::Vector3f(0, -1, 0));
    }

    void Player::Action()
    {
        auto cubes = Common::RayCast::GetResult(this->_camera, 50);

//        for (auto it = cubes.begin(), ite = cubes.end(); it != ite; ++it)
//        {
//            std::cout << "CHUNK " << Map::Chunk::CoordsToId(it->world) << ": " <<
//                it->chunk.x << ", " << it->chunk.y << ", " << it->chunk.z << "\n";
//        }
//
//        std::cout << "\n-------\n\n";

        Common::CubePosition cubePos;

        if (!this->_game.GetMap().GetFirstCube(cubes, cubePos))
        {
            cubePos = Common::CubePosition(this->_camera.position.world, this->_camera.position.chunk);
//            std::cout << "FAIL: " << Map::Chunk::CoordsToId(cubePos.world) << ": " <<
//                cubePos.chunk.x << ", " << cubePos.chunk.y << ", " << cubePos.chunk.z << "\n";
        }


        this->_game.GetClient().GetNetwork().SendUdpPacket(
            Network::PacketCreator::Action(this->_game.GetClient().GetClientId(), this->_camera, cubePos)
            );
    }

    void Player::SuperAction()
    {
        auto cubes = Common::RayCast::GetResult(this->_camera, 50);

        for (auto it = cubes.begin(), ite = cubes.end(); it != ite; ++it)
        {
            this->_game.GetClient().GetNetwork().SendUdpPacket(
                Network::PacketCreator::Action(this->_game.GetClient().GetClientId(), this->_camera, *it)
                );
        }
    }

    void Player::ToggleSprint()
    {
        this->_sprint = !this->_sprint;
    }

    void Player::_Move(Tools::Vector3f moveVector)
    {
        moveVector.Normalize();
        moveVector *= this->_GetSpeed();
        Common::Position p = this->_camera.position;
        p += moveVector;
        this->SetPosition(p);
        this->_moved = true;
    }

    float Player::_GetSpeed()
    {
        return this->_elapsedTime / (this->_sprint ? 10.0f : 200.0f);
    }

}}
