#ifndef __COMMON_PHYSICS_BODYSERIALIZER_HPP__
#define __COMMON_PHYSICS_BODYSERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "common/physics/Body.hpp"

namespace Common { namespace Physics {

    template<typename TBody>
        struct BodySerializer
        {

            static void Write(TBody const& b, Tools::ByteArray& p) // Used by Packet::Write<T>(T const&)
            {
                std::vector<Body::BodyNode> const& nodes = b.GetNodes();
                p.Write8(nodes.size());
                for (auto& node: nodes)
                {
                    if (node.dirty == true)
                    {
                        p.Write(true);
                        p.Write(node.node);
                    }
                    else
                    {
                        p.Write(false);
                    }
                }
            }

            static std::unique_ptr<std::vector<std::pair<bool, Node>>> Deserialize(Tools::ByteArray const& p)
            {
                unsigned int size = p.Read8();
                std::vector<std::pair<bool, Node>>* res = new std::vector<std::pair<bool, Node>>(size);
                for (unsigned int i = 0; i < size; ++i)
                {
                    p.Read((*res)[i].first);
                    if ((*res)[i].first)
                        p.Read((*res)[i].second);
                }
                return std::unique_ptr<std::vector<std::pair<bool, Node>>>(res);
            }

            private:
            static void Read(Tools::ByteArray const& p, TBody& b);  // Used by Packet::Read<T>(T&)

            private:
            static std::unique_ptr<TBody> Read(Tools::ByteArray const& p); // Used by Packet::Read<T>()
        };

}}

#endif
