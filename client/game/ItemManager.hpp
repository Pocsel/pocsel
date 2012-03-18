#ifndef __CLIENT_GAME_ITEMMANAGER_HPP__
#define __CLIENT_GAME_ITEMMANAGER_HPP__

namespace Common {
    struct OrientedPosition;
    struct MovingOrientedPosition;
}

namespace Client { namespace Game {
    class Game;
    class ItemRenderer;
}}

namespace Client { namespace Game {

    class ItemManager
    {
    private:
        Game& _game;
        Common::OrientedPosition* _pos;
        ItemRenderer* _renderer;

    public:
        ItemManager(Game& game);
        ~ItemManager();
        void MoveItem(Uint32 id, Common::MovingOrientedPosition const& pos);
        void Render();
        void Update(Uint32 time);
    };

}}

#endif
