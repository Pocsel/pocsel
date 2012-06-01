#ifndef __SERVER_NETWORK_BODYTYPESERIALIZER_HPP__
#define __SERVER_NETWORK_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/StdVectorSerializer.hpp"

#include "server/game/engine/BodyType.hpp"

namespace Tools {

    template<typename T> struct ByteArray::Serializer< Server::Game::Engine::BodyType >
    {

        static void Write(Server::Game::Engine::BodyType const& bt, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(bt.GetShapes());
        }

    private:
        static void Read(ByteArray const& p, Server::Game::Engine::BodyType& bt);  // Used by Packet::Read<T>(T&)
        static std::unique_ptr<Server::Game::Engine::BodyType> Read(ByteArray const& p); // Used by Packet::Read<T>()
    };

    template<typename T> struct ByteArray::Serializer< Server::Game::Engine::BodyType::ShapeNode >
    {

        static void Write(Server::Game::Engine::BodyType::ShapeNode const& sn, ByteArray& p) // Used by Packet::Write<T>(T const&)
        {
            p.Write(sn.name);
            p.Write(sn.children);
            p.Write(sn.parent);
        }

    private:
        static void Read(ByteArray const& p, Server::Game::Engine::BodyType::ShapeNode& sn);  // Used by Packet::Read<T>(T&)
        static std::unique_ptr<Server::Game::Engine::BodyType::ShapeNode> Read(ByteArray const& p); // Used by Packet::Read<T>()
    };

}

#endif

