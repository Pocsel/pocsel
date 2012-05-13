#ifndef __TOOLS_MODELS_UTILS_HPP__
#define __TOOLS_MODELS_UTILS_HPP__

#include "tools/models/Iqm.hpp"

namespace Tools { namespace Models { namespace Utils {

    template<typename T>
        struct UnionFind
        {
            struct Ufval
            {
                int rank, next;
                T val;

                Ufval(T const& val) : rank(0), next(-1), val(val) {}
            };

            std::vector<Ufval> ufvals;

            void Clear() { ufvals.clear(); }

            T const& Find(int k, T const& noval, T const& initval)
            {
                if (k >= (int)ufvals.size())
                    return initval;
                while (ufvals[k].next >= 0)
                    k = ufvals[k].next;
                if (ufvals[k].val == noval)
                    ufvals[k].val = initval;
                return ufvals[k].val;
            }

            int CompressFind(int k)
            {
                if (ufvals[k].next < 0)
                    return k;
                return ufvals[k].next = CompressFind(ufvals[k].next);
            }

            void Unite(int x, int y, const T &noval)
            {
                while ((int)ufvals.size() <= std::max(x, y))
                    ufvals.push_back(Ufval(noval));
                x = CompressFind(x);
                y = CompressFind(y);
                if (x == y)
                    return;
                Ufval& xval = ufvals[x];
                Ufval& yval = ufvals[y];
                if (xval.rank < yval.rank)
                    xval.next = y;
                else
                {
                    yval.next = x;
                    if (xval.rank == yval.rank)
                        yval.rank++;
                }
            }
        };

    int FindVarrayType(char const* name);

    int FindVarrayFormat(char const* name);
    int GetVarrayFormatSize(int format);

    struct TriangleInfo
    {
        bool used;
        float score;
        unsigned int vert[3];

        TriangleInfo() {}
        TriangleInfo(unsigned int v0, unsigned int v1, unsigned int v2)
        {
            vert[0] = v0;
            vert[1] = v1;
            vert[2] = v2;
        }
    };

    struct SharedVert
    {
        int index;
        int weld;

        SharedVert() {}
        SharedVert(int index, int weld) : index(index), weld(weld) {}

        bool operator < (SharedVert const& other) const
        {
            return index < other.index ||
                (index == other.index && weld < other.weld);
        }

        template<int TYPE> int RemapIndex(int) const;
    };

    struct VertexCache
    {
        enum
        {
            MaxVcache = 32
        };

        int index, rank;
        float score;
        int numuses;
        TriangleInfo** uses;

        VertexCache() : index(-1), rank(-1), score(-1.0f), numuses(0), uses(0) {}

        void CalcScore()
        {
            if (numuses > 0)
            {
                score = 2.0f * std::pow(numuses, -0.5f);
                if (rank >= 3)
                    score += std::pow(1.0f - (rank - 3)/float(MaxVcache - 3), 1.5f);
                else if (rank >= 0)
                    score += 0.75f;
            }
            else
                score = -1.0f;
        }

        void RemoveUse(TriangleInfo *t)
        {
            for (int i = 0; i < numuses; ++i)
            {
                if (uses[i] == t)
                {
                    uses[i] = uses[--numuses];
                    return;
                }
            }
        }
    };

    struct NeighborKey
    {
        unsigned int e0, e1;

        NeighborKey() {}
        NeighborKey(unsigned int e0, unsigned int e1) : e0(std::min(e0, e1)), e1(std::max(e0, e1)) {}

        bool operator < (NeighborKey const& other) const
        {
            return e0 < other.e0 || (e0 == other.e0 && e1 < other.e1);
        }
    };

    struct NeighborVal
    {
        unsigned int tris[2];

        NeighborVal() {}
        NeighborVal(unsigned int i) { tris[0] = i; tris[1] = 0xFFFFFFFFU; }

        void Add(unsigned int i)
        {
            if (tris[1] != 0xFFFFFFFFU)
                tris[0] = tris[1] = 0xFFFFFFFFU;
            else if (tris[0] != 0xFFFFFFFFU)
                tris[1] = i;
        }

        int Opposite(unsigned int i) const
        {
            return tris[0] == i ? tris[1] : tris[0];
        }
    };

    struct Ejoint
    {
        std::string name;
        int parent;

        Ejoint() : name(""), parent(-1) {}
    };

    struct Eanim
    {
        std::string name;
        int startframe, endframe;
        double fps;
        Uint32 flags;

        Eanim() : name(""), startframe(0), endframe(INT_MAX), fps(0), flags(0) {}
    };

    struct Emesh
    {
        std::string name;
        std::string material;
        int firsttri;
        bool used;

        Emesh() : name(""), material(""), firsttri(0), used(false) {}
        Emesh(std::string const& name, std::string const& material, int firsttri = 0) : name(name), material(material), firsttri(firsttri), used(false) {}
    };

    struct Evarray
    {
        std::string name;
        int type, format, size;

        Evarray() : type(Iqm::VertexArrayType::Position), format(Iqm::VertexArrayFormat::Float), size(3) { name[0] = '\0'; }
        Evarray(int type, int format, int size, std::string const& initname = "") : name(initname), type(type), format(format), size(size) {}
    };

    struct Esmoothgroup
    {
        enum
        {
            F_USED     = 1<<0,
            F_UVSMOOTH = 1<<1
        };

        int key;
        float angle;
        int flags;

        Esmoothgroup() : key(-1), angle(-1), flags(0) {}
    };

    struct Etriangle
    {
        int smoothgroup;
        uint vert[3], weld[3];

        Etriangle()
            : smoothgroup(-1)
        {
        }
        Etriangle(int v0, int v1, int v2, int smoothgroup = -1)
            : smoothgroup(smoothgroup)
        {
            vert[0] = v0;
            vert[1] = v1;
            vert[2] = v2;
        }
    };
    struct BlendCombo
    {
        int sorted;
        double weights[4];
        Uint8 bones[4];

        BlendCombo() : sorted(0) {}

        void Reset() { sorted = 0; }

        void AddWeight(double weight, int bone)
        {
            if(weight <= 1e-3) return;
            for (int k = 0; k<(sorted); ++k) if(weight > weights[k])
            {
                for(int l = std::min(sorted-1, 2); l >= k; l--)
                {
                    weights[l+1] = weights[l];
                    bones[l+1] = bones[l];
                }
                weights[k] = weight;
                bones[k] = bone;
                if(sorted<4) sorted++;
                return;
            }
            if(sorted>=4) return;
            weights[sorted] = weight;
            bones[sorted] = bone;
            sorted++;
        }

        void Finalize()
        {
            for (int j = 0; j<(4-sorted); ++j) { weights[sorted+j] = 0; bones[sorted+j] = 0; }
            if(sorted <= 0) return;
            double total = 0;
            for (int j = 0; j<(sorted); ++j) total += weights[j];
            total = 1.0/total;
            for (int j = 0; j<(sorted); ++j) weights[j] *= total;
        }

        void Serialize(Uint8 *vweights) const
        {
            int total = 0;
            for (unsigned int k = 0; k<(4); ++k) total += (vweights[k] = Uint8(weights[k]*255));
            if(sorted <= 0) return;
            while(total > 255)
            {
                for (unsigned int k = 0; k<(4); ++k) if(vweights[k] > 0 && total > 255) { vweights[k]--; total--; }
            }
            while(total < 255)
            {
                for (unsigned int k = 0; k<(4); ++k) if(vweights[k] < 255 && total < 255) { vweights[k]++; total++; }
            }
        }

        bool operator==(const BlendCombo &c) { for (unsigned int i = 0; i<(4); ++i) if(weights[i] != c.weights[i] || bones[i] != c.bones[i]) return false; return true; }
        bool operator!=(const BlendCombo &c) { for (unsigned int i = 0; i<(4); ++i) if(weights[i] != c.weights[i] || bones[i] != c.bones[i]) return true; return false; }
    };

    void IgnoreSpaces(char*& c);
    bool ParseIndex(char*& c, Int32& val);
    double ParseAttrib(char*& c, double ival = 0);
    bool MaybeParseAttrib(char*& c, double& result);
    char* TrimName(char*& c);
    glm::dvec4 ParseAttribs4(char*& c, glm::dvec4 const& ival = glm::dvec4(0, 0, 0, 0));
    glm::dvec3 ParseAttribs3(char*& c, glm::dvec3 const& ival = glm::dvec3(0, 0, 0));
    BlendCombo ParseBlends(char*& c);


    template<class T, class U>
        static inline void PutAttrib(T &out, const U &val) { out = T(val); }

    template<class T, class U>
        static inline void ScaleAttrib(T &out, const U &val) { PutAttrib(out, val); }
    template<class U>
        static inline void ScaleAttrib(Int8 &out, const U &val) { out = Int8(glm::clamp(val*255.0 - 0.5, -128.0, 127.0)); }
    template<class U>
        static inline void ScaleAttrib(Int16 &out, const U &val) { out = Int16(glm::clamp(val*65535.0 - 0.5, -32768.0, 32767.0)); }
    template<class U>
        static inline void ScaleAttrib(Int32 &out, const U &val) { out = Int32(glm::clamp(val*4294967295.0 - 0.5, -2147483648.0, 2147483647.0)); }
    template<class U>
        static inline void ScaleAttrib(Uint8 &out, const U &val) { out = Uint8(glm::clamp(val*255.0, 0.0, 255.0)); }
    template<class U>
        static inline void ScaleAttrib(Uint16 &out, const U &val) { out = Uint16(glm::clamp(val*65535.0, 0.0, 65535.0)); }
    template<class U>
        static inline void ScaleAttrib(Uint32 &out, const U &val) { out = Uint32(glm::clamp(val*4294967295.0, 0.0, 4294967295.0)); }

    template<int T>
        static inline bool NormalizedAttrib() { return true; }

    template<int TYPE, int FMT, class T, class U>
        static inline void SerializeAttrib(Iqm::VertexArray const& va, T* data, U const& attrib)
        {
            static_assert(sizeof(U) == 0, "le truc pas défaut est pas sensé être utilisé");
        }

    template<int TYPE, int FMT, class T, class GLM_T>
        static inline void SerializeAttrib(Iqm::VertexArray const& va, T* data, typename glm::detail::tvec4<GLM_T> const& attrib)
        {
            if (NormalizedAttrib<TYPE>()) switch (va.size)
            {
                case 4: ScaleAttrib(data[3], attrib.w);
                case 3: ScaleAttrib(data[2], attrib.z);
                case 2: ScaleAttrib(data[1], attrib.y);
                case 1: ScaleAttrib(data[0], attrib.x);
            }
            else switch (va.size)
            {
                case 4: PutAttrib(data[3], attrib.w);
                case 3: PutAttrib(data[2], attrib.z);
                case 2: PutAttrib(data[1], attrib.y);
                case 1: PutAttrib(data[0], attrib.x);
            }
            //lilswap(data, va.size);
        }

    template<int TYPE, int FMT, class T, class GLM_T>
        static inline void SerializeAttrib(Iqm::VertexArray const& va, T* data, typename glm::detail::tvec3<GLM_T> const& attrib)
        {
            if (NormalizedAttrib<TYPE>()) switch (va.size)
            {
                case 3: ScaleAttrib(data[2], attrib.z);
                case 2: ScaleAttrib(data[1], attrib.y);
                case 1: ScaleAttrib(data[0], attrib.x);
            }
            else switch (va.size)
            {
                case 3: PutAttrib(data[2], attrib.z);
                case 2: PutAttrib(data[1], attrib.y);
                case 1: PutAttrib(data[0], attrib.x);
            }
            //lilswap(data, va.size);
        }

    template<int TYPE, int FMT, class T>
        static inline void SerializeAttrib(Iqm::VertexArray const& va, T* data, BlendCombo const& blend)
        {
            if (TYPE == Iqm::VertexArrayType::BlendIndexes)
            {
                switch (va.size)
                {
                    case 4: PutAttrib(data[3], blend.bones[3]);
                    case 3: PutAttrib(data[2], blend.bones[2]);
                    case 2: PutAttrib(data[1], blend.bones[1]);
                    case 1: PutAttrib(data[0], blend.bones[0]);
                }
            }
            else if(FMT == Iqm::VertexArrayFormat::Ubyte)
            {
                Uint8 weights[4];
                blend.Serialize(weights);
                switch (va.size)
                {
                    case 4: PutAttrib(data[3], weights[3]);
                    case 3: PutAttrib(data[2], weights[2]);
                    case 2: PutAttrib(data[1], weights[1]);
                    case 1: PutAttrib(data[0], weights[0]);
                }
            }
            else
            {
                switch (va.size)
                {
                    case 4: ScaleAttrib(data[3], blend.weights[3]);
                    case 3: ScaleAttrib(data[2], blend.weights[2]);
                    case 2: ScaleAttrib(data[1], blend.weights[1]);
                    case 1: ScaleAttrib(data[0], blend.weights[0]);
                }
            }
            //lilswap(data, va.size);
        }
}}}

#endif
