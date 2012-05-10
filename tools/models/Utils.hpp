#ifndef __TOOLS_MODELS_UTILS_HPP__
#define __TOOLS_MODELS_UTILS_HPP__

#include "tools/models/Iqm.hpp"

namespace Tools { namespace Models {

    class Utils
    {
    public:
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

        static int FindVarrayType(const char *name)
        {
            static const struct _VarrayType
            {
                const char *name;
                int code;
            } _vatypes[] =
            {
                { "position", Iqm::VertexArrayType::Position },
                { "texcoord", Iqm::VertexArrayType::Texcoord },
                { "normal", Iqm::VertexArrayType::Normal  },
                { "tangent", Iqm::VertexArrayType::Tangent },
                { "blendindexes", Iqm::VertexArrayType::BlendIndexes },
                { "blendweights", Iqm::VertexArrayType::BlendWeights },
                { "color", Iqm::VertexArrayType::Color },
                { "custom0", Iqm::VertexArrayType::Custom + 0 },
                { "custom1", Iqm::VertexArrayType::Custom + 1 },
                { "custom2", Iqm::VertexArrayType::Custom + 2 },
                { "custom3", Iqm::VertexArrayType::Custom + 3 },
                { "custom4", Iqm::VertexArrayType::Custom + 4 },
                { "custom5", Iqm::VertexArrayType::Custom + 5 },
                { "custom6", Iqm::VertexArrayType::Custom + 6 },
                { "custom7", Iqm::VertexArrayType::Custom + 7 },
                { "custom8", Iqm::VertexArrayType::Custom + 8 },
                { "custom9", Iqm::VertexArrayType::Custom + 9 }
            };

            for (unsigned int i = 0; i < sizeof(_vatypes)/sizeof(_vatypes[0]); ++i)
            {
                if (!strcasecmp(_vatypes[i].name, name))
                    return _vatypes[i].code;
            }
            return -1;
        }

        static int FindVarrayFormat(const char *name)
        {
            static const struct _VarrayFormat
            {
                const char *name;
                int code;
                int size;
            } _vaformats[] =
            {
                { "byte", Iqm::VertexArrayFormat::Byte, 1 },
                { "ubyte", Iqm::VertexArrayFormat::Ubyte, 1 },
                { "short", Iqm::VertexArrayFormat::Short, 2 },
                { "ushort", Iqm::VertexArrayFormat::Ushort, 2 },
                { "int", Iqm::VertexArrayFormat::Int, 4 },
                { "uint", Iqm::VertexArrayFormat::Uint, 4 },
                { "half", Iqm::VertexArrayFormat::Half, 2 },
                { "float", Iqm::VertexArrayFormat::Float, 4 },
                { "double", Iqm::VertexArrayFormat::Double, 8 }
            };

            for (unsigned int i = 0; i < sizeof(_vaformats)/sizeof(_vaformats[0]); ++i)
            {
                if (!strcasecmp(_vaformats[i].name, name))
                    return _vaformats[i].code;
            }
            return -1;
        }

        struct Ejoint
        {
            std::string name;
            int parent;

            Ejoint() : name(NULL), parent(-1) {}
        };

        struct Eanim
        {
            std::string name;
            int startframe, endframe;
            double fps;
            Uint32 flags;

            Eanim() : name(NULL), startframe(0), endframe(INT_MAX), fps(0), flags(0) {}
        };

        struct Emesh
        {
            std::string name;
            std::string material;
            int firsttri;
            bool used;

            Emesh() : name(NULL), material(NULL), firsttri(0), used(false) {}
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

        static void IgnoreSpaces(char*& c)
        {
            while (std::isspace(*c))
                ++c;
        }

        static bool ParseIndex(char*& c, Int32& val)
        {
            IgnoreSpaces(c);
            char* end = 0;
            Int32 rval = std::strtol(c, &end, 10);
            if (c == end)
                return false;
            val = rval;
            c = end;
            return true;
        }

        static double ParseAttrib(char*& c, double ival = 0)
        {
            IgnoreSpaces(c);
            char *end = NULL;
            double val = std::strtod(c, &end);
            if (c == end)
                val = ival;
            c = end;
            return val;
        }

        static bool MaybeParseAttrib(char*& c, double& result)
        {
            IgnoreSpaces(c);
            char *end = NULL;
            double val = std::strtod(c, &end);
            if (c == end)
                return false;
            c = end;
            result = val;
            return true;
        }

        static char* TrimName(char*& c)
        {
            IgnoreSpaces(c);
            char* start;
            char* end;
            if (*c == '"')
            {
                ++c;
                start = end = c;
                while (*end && *end != '"')
                    ++end;
                if (*end)
                {
                    *end = '\0';
                    ++end;
                }
            }
            else
            {
                start = end = c;
                while (*end && !isspace(*end))
                    ++end;
                if (*end)
                {
                    *end = '\0';
                    ++end;
                }
            }
            c = end;
            return start;
        }

        static glm::dvec4 ParseAttribs4(char*& c, glm::dvec4 const& ival = glm::dvec4(0, 0, 0, 0))
        {
            glm::dvec4 val;
            for (unsigned int k = 0; k < 4; ++k)
                val[k] = ParseAttrib(c, ival[k]);
            return val;
        }

        static glm::dvec3 ParseAttribs3(char*& c, glm::dvec3 const& ival = glm::dvec3(0, 0, 0))
        {
            glm::dvec3 val;
            for (unsigned int k = 0; k < 4; ++k)
                val[k] = ParseAttrib(c, ival[k]);
            return val;
        }

        static BlendCombo ParseBlends(char*& c)
        {
            BlendCombo b;
            int index;
            while (ParseIndex(c, index))
                b.AddWeight(ParseAttrib(c, 0), index);
            b.Finalize();
            return b;
        }
    };

}}

#endif
