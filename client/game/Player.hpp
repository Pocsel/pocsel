#ifndef __CLIENT_GAME_PLAYER_HPP__
#define __CLIENT_GAME_PLAYER_HPP__

#include "common/Camera.hpp"
#include "tools/Vector3.hpp"
#include "client/ActionBinder.hpp"

namespace Client { namespace Window {
    class InputManager;
    class Window;
}}

namespace Client { namespace Game {

    class Player :
        private boost::noncopyable
    {
        private:
            std::string _nickname;
            Common::Camera _camera;
            ActionBinder _actionBinder;
            Uint32 _elapsedTime;

        public:
            Player();
            std::string const& GetNickname() const { return this->_nickname; }
            void SetNickname(std::string const& nick) { this->_nickname = nick; }
            Common::Position const& GetPosition() const { return this->_camera.position; }
            void SetPosition(Common::Position const& pos);
            Common::Camera& GetCamera() { return this->_camera; }
            Common::Camera const& GetCamera() const { return this->_camera; }

            void UpdateMovements(Window::Window& window, Uint32 time);
            void MoveForward();
            void MoveBackward();
            void StrafeLeft();
            void StrafeRight();
            void Jump();

        private:
            void _Move(Tools::Vector3f moveVector);
    };

}}

#endif
