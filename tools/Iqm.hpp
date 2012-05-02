#ifndef __TOOLS_IQM_HPP__
#define __TOOLS_IQM_HPP__

namespace Tools { namespace Iqm {

    static char const* const Magic = "INTERQUAKEMODEL";
    static Uint32 const Version = 2;

    struct Header
    {
        char magic[16];
        Uint32 version;
        Uint32 filesize;
        Uint32 flags;
        Uint32 num_text, ofs_text;
        Uint32 num_meshes, ofs_meshes;
        Uint32 num_vertexarrays, num_vertexes, ofs_vertexarrays;
        Uint32 num_triangles, ofs_triangles, ofs_adjacency;
        Uint32 num_joints, ofs_joints;
        Uint32 num_poses, ofs_poses;
        Uint32 num_anims, ofs_anims;
        Uint32 num_frames, num_framechannels, ofs_frames, ofs_bounds;
        Uint32 num_comment, ofs_comment;
        Uint32 num_extensions, ofs_extensions;
    };

    struct Mesh
    {
        Uint32 name;
        Uint32 material;
        Uint32 first_vertex, num_vertexes;
        Uint32 first_triangle, num_triangles;
    };

    namespace VertexArrayType {
        enum
        {
            Position     = 0,
            Texcoord     = 1,
            Normal       = 2,
            Tangent      = 3,
            BlendIndexes = 4,
            BlendWeights = 5,
            Color        = 6,
            Custom       = 0x10
        };
    }
    namespace VertexArrayFormat {
        enum
        {
            Byte   = 0,
            Ubyte  = 1,
            Short  = 2,
            Ushort = 3,
            Int    = 4,
            Uint   = 5,
            Half   = 6,
            Float  = 7,
            Double = 8,
        };
    }

    struct Triangle
    {
        Uint32 vertex[3];
    };

    struct Joint
    {
        Uint32 name;
        Int32 parent;
        glm::fvec3 position;
        glm::fquat orientation;
        glm::fvec3 size;
    };

    struct Pose
    {
        Int32 parent;
        Uint32 mask;
        float channeloffset[10];
        float channelscale[10];
    };

    struct Anim
    {
        Uint32 name;
        Uint32 first_frame, num_frames;
        float framerate;
        Uint32 flags;
    };

    struct VertexArray
    {
        Uint32 type;
        Uint32 flags;
        Uint32 format;
        Uint32 size;
        Uint32 offset;
    };

    struct Bounds
    {
        float bbmin[3], bbmax[3];
        float xyradius, radius;
    };

}}

#endif
