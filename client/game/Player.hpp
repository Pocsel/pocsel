#ifndef __CLIENT_GAME_PLAYER_HPP__
#define __CLIENT_GAME_PLAYER_HPP__

#include "common/Camera.hpp"
#include "tools/Vector3.hpp"
#include "client/ActionBinder.hpp"

namespace Common {
    struct CubePosition;
}

namespace Client { namespace Window {
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
            ActionBinder _actionBinder;
            Uint32 _elapsedTime;
            bool _moved;
            Uint32 _movedTime;
            bool _sprint;
            Common::CubePosition* _targetedCube;
            TargetedCubeRenderer* _targetedCubeRenderer;

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

            void UpdateMovements(Uint32 time);
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
            void _Move(Tools::Vector3f moveVector);
            float _GetSpeed();
    };

}}

#endif
