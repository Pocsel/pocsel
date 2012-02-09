#ifndef __SERVER_GAME_GAME_HPP__
#define __SERVER_GAME_GAME_HPP__

namespace Server {

    class Server;

}

namespace Server { namespace Game {

    class World;

    class Game :
        private boost::noncopyable
    {
    private:
        Server& _server;
        World* _world;

    public:
        Game(Server& server);
        ~Game();

        void Start();
        void Stop();

        World const& GetWorld() const { return *this->_world; }
        World& GetWorld() { return *this->_world; }
    };

}}

#endif
