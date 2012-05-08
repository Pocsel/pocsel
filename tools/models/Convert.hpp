#ifndef __TOOLS_MODELS_CONVERT_HPP__
#define __TOOLS_MODELS_CONVERT_HPP__

#include "tools/models/Iqm.hpp"

namespace Tools { namespace Models {

    class Convert
    {
    private:
        struct Transform
        {
            glm::vec3 pos;
            glm::quat orient;
            glm::vec3 scale;

            Transform() {}
            Transform(glm::vec3 const& pos, glm::quat const& orient, glm::vec3 const& scale = glm::vec3(1, 1, 1)) :
                pos(pos), orient(orient), scale(scale)
            {}
        };
    private:
        std::vector<Iqm::Triangle> _triangles;
        std::vector<Iqm::Triangle> _neighbors;

        std::vector<Iqm::Mesh> _meshes;

        std::vector<Iqm::Anim> _anims;

        std::vector<Iqm::Joint> _joints;

        std::vector<Iqm::Pose> _poses;

        //std::vector<Iqm::Bounds> bounds;
        std::vector<Transform> _frames;

        std::vector<char> _stringData;
        //std::vector<char> _commentData;

        std::map<std::string, Uint32> _sharedStrings;

    public:

    private:
        Uint32 _ShareString(std::string const& str);
        Uint32 _PushString(std::string const& str);

        bool _LoadIqe(std::string const& path);
    };

}}

#endif
