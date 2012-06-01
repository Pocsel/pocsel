#ifndef __CLIENT_NETWORK_BODYTYPESERIALIZER_HPP__
#define __CLIENT_NETWORK_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/StdVectorSerializer.hpp"

#include "client/game/engine/BodyType.hpp"

namespace Tools {

    template<typename T> struct ByteArray::Serializer< Client::Game::Engine::BodyType >
    {

        static std::unique_ptr<Client::Game::Engine::BodyType> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
            std::vector<Client::Game::Engine::BodyType::ShapeNode> vec;
            p.Read(vec);
            return std::unique_ptr<Client::Game::Engine::BodyType>(new Client::Game::Engine::BodyType(vec));
        }

    private:
        static void Write(Client::Game::Engine::BodyType const& bt, ByteArray& p); // Used by Packet::Write<T>(T const&)
        static void Read(ByteArray const& p, Client::Game::Engine::BodyType& bt);  // Used by Packet::Read<T>(T&)
    };

    template<typename T> struct ByteArray::Serializer< Client::Game::Engine::BodyType::ShapeNode >
    {

        static void Read(ByteArray const& p, Client::Game::Engine::BodyType::ShapeNode& sn)  // Used by Packet::Read<T>(T&)
        {
            p.Read(sn.name);
            p.Read(sn.children);
            p.Read(sn.parent);
        }

    private:
        static void Write(Client::Game::Engine::BodyType::ShapeNode const& sn, ByteArray& p); // Used by Packet::Write<T>(T const&)
        static std::unique_ptr<Client::Game::Engine::BodyType::ShapeNode> Read(ByteArray const& p); // Used by Packet::Read<T>()
    };

}

#endif


