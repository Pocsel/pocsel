#include "tools/models/Utils.hpp"

namespace Tools { namespace Models { namespace Utils {

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
        { "custom9", Iqm::VertexArrayType::Custom + 9 },
        {0, 0}
    };
    int FindVarrayType(const char *name)
    {
        for (unsigned int i = 0; _vatypes[i].name != 0;  ++i)
        {
            if (!strcasecmp(_vatypes[i].name, name))
                return _vatypes[i].code;
        }
        return -1;
    }

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
        { "double", Iqm::VertexArrayFormat::Double, 8 },
        {0, 0, 0}
    };

    int FindVarrayFormat(const char *name)
    {
        for (unsigned int i = 0; _vaformats[i].name != 0; ++i)
        {
            if (!strcasecmp(_vaformats[i].name, name))
                return _vaformats[i].code;
        }
        return -1;
    }

    int GetVarrayFormatSize(int format)
    {
        return _vaformats[format].size;
    }

    void IgnoreSpaces(char*& c)
    {
        while (std::isspace(*c))
            ++c;
    }

    bool ParseIndex(char*& c, Int32& val)
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

    double ParseAttrib(char*& c, double ival)
    {
        IgnoreSpaces(c);
        char *end = NULL;
        double val = std::strtod(c, &end);
        if (c == end)
            val = ival;
        c = end;
        return val;
    }

    bool MaybeParseAttrib(char*& c, double& result)
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

    char* TrimName(char*& c)
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

    glm::dvec4 ParseAttribs4(char*& c, glm::dvec4 const& ival)
    {
        glm::dvec4 val;
        for (unsigned int k = 0; k < 4; ++k)
            val[k] = ParseAttrib(c, ival[k]);
        return val;
    }

    glm::dvec3 ParseAttribs3(char*& c, glm::dvec3 const& ival)
    {
        glm::dvec3 val;
        for (unsigned int k = 0; k < 3; ++k)
            val[k] = ParseAttrib(c, ival[k]);
        return val;
    }

    BlendCombo ParseBlends(char*& c)
    {
        BlendCombo b;
        int index;
        while (ParseIndex(c, index))
            b.AddWeight(ParseAttrib(c, 0), index);
        b.Finalize();
        return b;
    }

    template<int TYPE> int SharedVert::RemapIndex(int) const
    {
        return index;
    }
    template<> int SharedVert::RemapIndex<Iqm::VertexArrayType::Normal>(int) const
    {
        return weld;
    }
    template<> int SharedVert::RemapIndex<Iqm::VertexArrayType::Tangent>(int i) const
    {
        return i;
    }

    template int SharedVert::RemapIndex<Iqm::VertexArrayType::Normal>(int) const;
    template int SharedVert::RemapIndex<Iqm::VertexArrayType::Tangent>(int i) const;

    template int SharedVert::RemapIndex<Iqm::VertexArrayType::Position>(int i) const;
    template int SharedVert::RemapIndex<Iqm::VertexArrayType::Texcoord>(int i) const;
    template int SharedVert::RemapIndex<Iqm::VertexArrayType::BlendIndexes>(int i) const;
    template int SharedVert::RemapIndex<Iqm::VertexArrayType::BlendWeights>(int i) const;
    template int SharedVert::RemapIndex<Iqm::VertexArrayType::Color>(int i) const;
}}}
