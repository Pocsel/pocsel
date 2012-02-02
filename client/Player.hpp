#ifndef __CLIENT_PLAYER_HPP__
#define __CLIENT_PLAYER_HPP__

#include <boost/noncopyable.hpp>
#include <string>

#include "common/Camera.hpp"

namespace Tools { namespace Gui {
    class EventManager;
    namespace Events {
        struct BindAction;
    }
}}

namespace Common {
    class Packet;
}

namespace Client
{

    class PacketDispatcher;

    class Player :
        private boost::noncopyable
    {
        private:
            std::string _nick;
            Common::Camera _camera;
            Tools::Gui::EventManager& _eventManager;

        public:
            Player(Tools::Gui::EventManager& eventManager, PacketDispatcher& packetDispatcher);
            std::string const& GetNick() const { return this->_nick; }
            void SetNick(std::string const& nick) { this->_nick = nick; }
            Common::Position const& GetPosition() const { return this->_camera.position; }
            Common::Camera& GetCamera() { return this->_camera; }
            Common::Camera const& GetCamera() const { return this->_camera; }
            void SetPosition(Common::Position const& pos);

            void MoveForward();
            void MoveBackward();
            void StrafeLeft();
            void StrafeRight();
            void Jump();

        private:
            void _Move(Tools::Vector3f moveVector);
            void _HandleSpawnPosition(Common::Packet const& p);
            void _OnBindJump(Tools::Gui::Events::BindAction const& e);
    };

}

#endif
