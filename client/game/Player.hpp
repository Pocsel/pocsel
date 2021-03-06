#ifndef __CLIENT_GAME_PLAYER_HPP__
#define __CLIENT_GAME_PLAYER_HPP__

#include "common/Camera.hpp"
#include "tools/Vector3.hpp"
#include "tools/window/ActionBinder.hpp"
#include "common/CubePosition.hpp"

namespace Tools { namespace Window {
    class InputManager;
    class Window;
}}

namespace Client { namespace Game {

    class Game;
    class TargetedCubeRenderer;

    class Player :
        private boost::noncopyable
    {
        private:
            Game& _game;
            std::string _nickname;
            Common::Camera _camera;
            Tools::Window::ActionBinder _actionBinder;
            float _elapsedTime;
            bool _moved;
            Uint32 _movedTime;
            bool _sprint;
            Common::CubePosition* _targetedCube;
            TargetedCubeRenderer* _targetedCubeRenderer;
            glm::fvec3 _movement;

        public:
            Player(Game& game);
            ~Player();
            std::string const& GetNickname() const { return this->_nickname; }
            void SetNickname(std::string const& nick) { this->_nickname = nick; }
            Common::Position const& GetPosition() const { return this->_camera.position; }
            void SetPosition(Common::Position const& pos);
            Common::Camera& GetCamera() { return this->_camera; }
            Common::Camera const& GetCamera() const { return this->_camera; }
            bool HasTargetedCube() const { return this->_targetedCube != 0; }
            Common::CubePosition const& GetTargetedCube() const { assert(this->_targetedCube != 0); return *this->_targetedCube; }
            void Render();

            void UpdateMovements(Uint64 deltaTime);
            void MoveForward();
            void MoveBackward();
            void StrafeLeft();
            void StrafeRight();
            void Jump();
            void Crouch();
            void Action();
            void SuperAction();
            void ToggleSprint();

        private:
            void _Move(glm::fvec3 moveVector);
            void _Move();
            float _GetSpeed();
    };

}}

#endif
