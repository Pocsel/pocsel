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

        std::vector<Utils::SharedVert> _vmap;
        std::vector<Iqm::VertexArray> _varrays;
        std::vector<Uint8> _vdata;

        int _framesize;
        std::vector<Uint16> _animdata;

        std::vector<glm::dvec4> _mpositions, _epositions, _etexcoords, /*_etangents, */_ecolors;//, _ecustom[10];
        std::vector<glm::dvec3> _enormals;//, _ebitangents;
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
        double _escale;

    public:
//        Convert() {
//            _escale = 1;
//                _framesize = 0;
//        }

    private:
        Uint32 _ShareString(std::string const& str);
        Uint32 _PushString(std::string const& str);

        void _MakeAnims();
        void _WeldVert(std::vector<glm::vec3> const& norms, WeldInfo* welds, int& numwelds, Utils::UnionFind<int>& welder);
        void _SmoothVerts(bool areaweight = true);
        void _MakeNeighbors();
        void _MakeTriangles(std::vector<Utils::TriangleInfo>& tris, std::vector<Utils::SharedVert> const& mmap);
        void _MakeMeshes();
        bool _LoadIqe(std::string const& path);

        void _CalcAnimData();
        bool _WriteMqm(std::string const& path);

        template<int TYPE, int FORMAT, class T>
            void _SetupVertexArray(std::vector<T> const& attribs, int size)
            {
                _varrays.push_back(Iqm::VertexArray(TYPE, FORMAT, size, _vdata.size()));
                Iqm::VertexArray& va = _varrays.back();
                std::string name;
                for (unsigned int i = 0; i < _evarrays.size(); ++i)
                {
                    if (_evarrays[i].type == (int)va.type)
                    {
                        Utils::Evarray &info = _evarrays[i];
                        va.format = info.format;
                        va.size = glm::clamp(info.size, 1, 4);
                        name = info.name;
                        break;
                    }
                }
                uint align = std::max(Utils::GetVarrayFormatSize(va.format), 4);
                if (va.offset%align)
                {
                    unsigned int pad = align - va.offset%align;
                    va.offset += pad;
                    for (unsigned int i = 0; i < pad; ++i)
                        _vdata.push_back(0);
                }
                //if (va.type >= IQM_CUSTOM)
                //{
                //    if(!name[0])
                //    {
                //        defformatstring(customname)("custom%d", va.type-IQM_CUSTOM);
                //        va.type = IQM_CUSTOM + sharestring(customname);
                //    }
                //    else va.type = IQM_CUSTOM + sharestring(name);
                //}

                int totalsize = Utils::GetVarrayFormatSize(va.format) * va.size * _vmap.size();
                size_t vdatasize = _vdata.size();
                _vdata.resize(totalsize);
                Uint8* data = _vdata.data() + vdatasize;
                for (unsigned int i = 0; i < _vmap.size(); ++i)
                {
                    const T &attrib = attribs[_vmap[i].RemapIndex<TYPE>(i)];
                    switch (va.format)
                    {
                        case Iqm::VertexArrayFormat::Byte:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Byte>(va, (Int8*)data, attrib);
                            break;
                        case Iqm::VertexArrayFormat::Ubyte:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Ubyte>(va, (Uint8*)data, attrib);
                            break;
                        case Iqm::VertexArrayFormat::Short:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Short>(va, (Int16*)data, attrib);
                            break;
                        case Iqm::VertexArrayFormat::Ushort:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Ushort>(va, (Uint16*)data, attrib);
                            break;
                        case Iqm::VertexArrayFormat::Int:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Int>(va, (Int32*)data, attrib);
                            break;
                        case Iqm::VertexArrayFormat::Uint:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Uint>(va, (Uint32*)data, attrib);
                            break;
                        case Iqm::VertexArrayFormat::Half:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Half>(va, (glm::half*)data, attrib);
                            break;
                        case Iqm::VertexArrayFormat::Float:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Float>(va, (float*)data, attrib);
                            break;
                        case Iqm::VertexArrayFormat::Double:
                            Utils::SerializeAttrib<TYPE, Iqm::VertexArrayFormat::Double>(va, (double*)data, attrib);
                            break;
                    }
                    data += Utils::GetVarrayFormatSize(va.format) * va.size;
                }
            }

    };

}}

#endif
