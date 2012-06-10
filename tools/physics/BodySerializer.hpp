#ifndef __TOOLS_PHYSICS_BODYSERIALIZER_HPP__
#define __TOOLS_PHYSICS_BODYSERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "tools/physics/Body.hpp"

namespace Tools { namespace Physics {

    template<typename TBody>
        struct BodySerializer
        {

            static void Write(TBody const& b, ByteArray& p) // Used by Packet::Write<T>(T const&)
            {
                std::vector<Body::BodyNode> const& nodes = b.GetNodes();
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
            static std::unique_ptr<TBody> Read(ByteArray const& p); // Used by Packet::Read<T>()

            private:
            static void Read(ByteArray const& p, TBody& b);  // Used by Packet::Read<T>(T&)
        };

}}

#endif
