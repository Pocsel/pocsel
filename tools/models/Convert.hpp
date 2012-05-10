#ifndef __TOOLS_MODELS_CONVERT_HPP__
#define __TOOLS_MODELS_CONVERT_HPP__

#include "tools/models/Iqm.hpp"
#include "tools/models/Utils.hpp"

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
        struct WeldInfo
        {
            int tri;
            int vert;
            WeldInfo *next;
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

        std::vector<glm::dvec4> _mpositions, _epositions, _etexcoords, _etangents, _ecolors, _ecustom[10];
        std::vector<glm::dvec3> _enormals, _ebitangents;
        std::vector<Utils::BlendCombo> _mblends, _eblends;
        std::vector<Utils::Etriangle> _etriangles;
        std::vector<Utils::Esmoothgroup> _esmoothgroups;
        std::vector<int> _esmoothindexes;
        std::vector<Uint8> _esmoothedges;
        std::vector<Utils::Ejoint> _ejoints;
        std::vector<Transform> _eposes;
        std::vector<glm::mat3x3> _mjoints;
        std::vector<int> _eframes;
        std::vector<Utils::Eanim> _eanims;
        std::vector<Utils::Emesh> _emeshes;
        std::vector<Utils::Evarray> _evarrays;
        std::map<const char *, char *> _enames;

    public:

    private:
        Uint32 _ShareString(std::string const& str);
        Uint32 _PushString(std::string const& str);

        void _WeldVert(std::vector<glm::vec3> const& norms, WeldInfo* welds, int& numwelds, Utils::UnionFind<int>& welder);
        void _SmoothVerts(bool areaweight = true);
        bool _LoadIqe(std::string const& path);
        bool _WriteMqm(std::string const& path);
    };

}}

#endif
