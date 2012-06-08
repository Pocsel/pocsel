#ifndef __SERVER_NETWORK_BODYSERIALIZER_HPP__
#define __SERVER_NETWORK_BODYSERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "server/game/engine/Body.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Server::Game::Engine::Body >
    {

        static void Write(Server::Game::Engine::Body const& b, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            std::vector<BodyNode> const& nodes = b.GetNodes();
            for (auto it = nodes.begin(), ite = nodes.end(); it != ite; ++it)
            {
                if (it->dirty == true)
                {
                    p.Write(true);
                    p.Write(it->node);
                }
                else
                {
                    p.Write(false);
                }
            }
        }

    private:
        static void Read(ByteArray const& p, Server::Game::Engine::Body& b);  // Used by Packet::Read<T>(T&)
        static std::unique_ptr<Server::Game::Engine::Body> Read(ByteArray const& p); // Used by Packet::Read<T>()
    };

}

#endif
