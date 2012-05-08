#include "tools/models/Convert.hpp"
#include "tools/models/Utils.hpp"
#include "tools/Math.hpp"

namespace Tools { namespace Models {

    Uint32 Convert::_ShareString(std::string const& str)
    {
        if (this->_sharedStrings.count(str))
            return this->_sharedStrings[str];
        return this->_sharedStrings[str] = this->_PushString(str);
    }

    Uint32 Convert::_PushString(std::string const& str)
    {
        Uint32 pos = this->_stringData.size();
        this->_stringData.resize(pos + str.size() + 1);
        std::memcpy(this->_stringData.data() + pos, str.c_str(), str.size() + 1);
        return pos;
    }

    bool Convert::_LoadIqe(std::string const& path)
    {
        std::ifstream f(path);
        if (!f)
        {
            Tools::error << "could not open " << path << "\n";
            return false;
        }

        std::vector<glm::dvec4> mpositions, epositions, etexcoords, etangents, ecolors, ecustom[10];
        std::vector<glm::dvec3> enormals, ebitangents;
        std::vector<Utils::BlendCombo> mblends, eblends;
        std::vector<Utils::Etriangle> etriangles;
        std::vector<Utils::Esmoothgroup> esmoothgroups;
        std::vector<int> esmoothindexes;
        std::vector<Uint8> esmoothedges;
        std::vector<Utils::Ejoint> ejoints;
        std::vector<Transform> eposes;
        std::vector<glm::mat3x3> mjoints;
        std::vector<int> eframes;
        std::vector<Utils::Eanim> eanims;
        std::vector<Utils::Emesh> emeshes;
        std::vector<Utils::Evarray> evarrays;
        std::map<const char *, char *> enames;

        char const* curmesh = 0;
        char const* curmaterial = 0;
        bool needmesh = true;
        int fmoffset = 0;
        std::vector<char> buf(4096);
        char* c;

        f.getline(buf.data(), buf.size(), '\n');
        if (!(c = std::strchr(buf.data(), '#')) || std::strstr(buf.data(), "# Inter-Quake Export") != c)
        {
            Tools::error << "Not an IQE: " << path << "\n";
            return false;
        }

        while (f.getline(buf.data(), buf.size(), '\n'))
        {
            c = buf.data();
            Utils::IgnoreSpaces(c);
            if (std::isalpha(c[0]) && std::isalnum(c[1]) && (!c[2] || std::isspace(c[2])))
            {
                switch (*c++)
                {
                case 'v':
                    switch (*c++)
                    {
                    case 'p':
                        epositions.push_back(Utils::ParseAttribs4(c, glm::dvec4(0, 0, 0, 1))); continue;
                    case 't':
                        etexcoords.push_back(Utils::ParseAttribs4(c)); continue;
                    case 'n':
                        enormals.push_back(Utils::ParseAttribs3(c)); continue;
                    case 'x':
                        {
                            glm::dvec4 tangent(Utils::ParseAttribs3(c), 0);
                            glm::dvec3 bitangent(0, 0, 0);
                            bitangent.x = Utils::ParseAttrib(c);
                            if (Utils::MaybeParseAttrib(c, bitangent.y))
                            {
                                bitangent.z = Utils::ParseAttrib(c);
                                ebitangents.push_back(bitangent);
                            }
                            else
                                tangent.w = bitangent.x;
                            etangents.push_back(tangent);
                            continue;
                        }
                    case 'b':
                        eblends.push_back(Utils::ParseBlends(c)); continue;
                    case 'c':
                        continue; //ecolors.push_back(parseattribs4(c, Vec4(0, 0, 0, 1))); continue;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        //ecustom[c[-1] - '0'].push_back(parseattribs4(c));
                        continue;
                    case 's':
                        int idx;
                        Utils::ParseIndex(c, idx);
                        esmoothindexes.push_back(idx);
                        continue;
                    }
                    break;
                case 'p':
                    {
                        Transform t;
                        switch(*c++)
                        {
                        case 'q':
                            {
                                t.pos = Utils::ParseAttribs3(c);
                                for (unsigned int k = 0; k < 3; ++k)
                                    t.orient[k] = Utils::ParseAttrib(c);
                                Math::ComputeQuatW(t.orient);
                                double w = Utils::ParseAttrib(c, t.orient.w);
                                if (w != t.orient.w)
                                {
                                    t.orient.w = w;
                                    t.orient = glm::normalize(t.orient);
                                }
                                //if (t.orient.w > 0) t.orient.flip(); XXX
                                t.scale = Utils::ParseAttribs3(c, glm::dvec3(1, 1, 1));
                                eposes.push_back(t);
                                continue;
                            }
                        case 'm':
                            {
                                t.pos = Utils::ParseAttribs3(c);
                                glm::mat3x3 m;
                                for (unsigned int k = 0; k < 3; ++k)
                                    m[k] = Utils::ParseAttribs3(c);
                                glm::dvec3 mscale(
                                        glm::length(glm::dvec3(m[0].x, m[1].x, m[2].x)),
                                        glm::length(glm::dvec3(m[0].y, m[1].y, m[2].y)),
                                        glm::length(glm::dvec3(m[0].z, m[1].z, m[2].z)));
                                // check determinant for sign of scaling
                                if (glm::determinant(m) < 0)
                                    mscale = -mscale;
                                for (unsigned int k = 0; k < 3; ++k)
                                    m[k] /= mscale;
                                t.orient = glm::toQuat(m);
                                //if (t.orient.w > 0) t.orient.flip(); XXX
                                t.scale = Utils::ParseAttribs3(c, glm::dvec3(1, 1, 1)) * mscale;
                                eposes.push_back(t);
                                continue;
                            }
                        case 'a':
                            {
                                t.pos = Utils::ParseAttribs3(c);
                                glm::dvec3 rot = Utils::ParseAttribs3(c);
                                double cx = std::cos(rot.x / 2), sx = std::sin(rot.x / 2),
                                       cy = std::cos(rot.y / 2), sy = std::sin(rot.y / 2),
                                       cz = std::cos(rot.z / 2), sz = std::sin(rot.z / 2);
                                t.orient = glm::quat(sx*cy*cz - cx*sy*sz,
                                        cx*sy*cz + sx*cy*sz,
                                        cx*cy*sz - sx*sy*cz,
                                        cx*cy*cz + sx*sy*sz);
                                //if (t.orient.w > 0) t.orient.flip(); XXX
                                t.scale = Utils::ParseAttribs3(c, glm::dvec3(1, 1, 1));
                                eposes.push_back(t);
                                continue;
                            }
                        }
                        break;
                    }
                case 'f':
                    switch (*c++)
                    {
                    case 'a':
                        {
                            int i1 = 0, i2 = 0, i3 = 0;
                            if (!Utils::ParseIndex(c, i1) || !Utils::ParseIndex(c, i2)) continue;
                            if (needmesh)
                            {
                                emeshes.push_back(Utils::Emesh(curmesh, curmaterial, etriangles.size()));
                                needmesh = false;
                            }
                            if (i1 < 0) i1 = std::max<int>(epositions.size() + i1, 0);
                            if (i2 < 0) i2 = std::max<int>(epositions.size() + i2, 0);
                            while (Utils::ParseIndex(c, i3))
                            {
                                if (i3 < 0) i3 = std::max<int>(epositions.size() + i3, 0);
                                esmoothgroups.back().flags |= Utils::Esmoothgroup::F_USED;
                                etriangles.push_back(Utils::Etriangle(i1, i2, i3, esmoothgroups.size()-1));
                                i2 = i3;
                            }
                            continue;
                        }
                    case 'm':
                        {
                            int i1 = 0, i2 = 0, i3 = 0;
                            if (!Utils::ParseIndex(c, i1) || !Utils::ParseIndex(c, i2)) continue;
                            if (needmesh)
                            {
                                emeshes.push_back(Utils::Emesh(curmesh, curmaterial, etriangles.size()));
                                needmesh = false;
                            }
                            i1 = i1 < 0 ? std::max<int>(epositions.size() + i1, 0) : (fmoffset + i1);
                            i2 = i2 < 0 ? std::max<int>(epositions.size() + i2, 0) : (fmoffset + i2);
                            while (Utils::ParseIndex(c, i3))
                            {
                                i3 = i3 < 0 ? std::max<int>(epositions.size() + i3, 0) : (fmoffset + i3);
                                esmoothgroups.back().flags |= Utils::Esmoothgroup::F_USED;
                                etriangles.push_back(Utils::Etriangle(i1, i2, i3, esmoothgroups.size()-1));
                                i2 = i3;
                            }
                            continue;
                        }
                    case 's':
                        {
                            int i1 = 0, i2 = 0, i3 = 0;
                            Uint8 flags = 0;
                            if (!Utils::ParseIndex(c, i1) || !Utils::ParseIndex(c, i2) || !Utils::ParseIndex(c, i3)) continue;
                            flags |= glm::clamp(i1, 0, 1);
                            flags |= glm::clamp(i2, 0, 1)<<1;
                            flags |= glm::clamp(i3, 0, 1)<<2;
                            esmoothgroups.back().flags |= Utils::Esmoothgroup::F_USED;
                            while (Utils::ParseIndex(c, i3))
                            {
                                esmoothedges.push_back(flags | 4);
                                flags = 1 | ((flags & 4) >> 1) | (glm::clamp(i3, 0, 1)<<2);
                            }
                            esmoothedges.push_back(flags);
                            continue;
                        }
                    }
                    break;
                }
            }

            char *args = c;
            while (*args && !isspace(*args)) args++;
            if (!std::strncmp(c, "smoothgroup", std::max(int(args-c), 11)))
            {
                if (esmoothgroups.back().flags & Utils::Esmoothgroup::F_USED) esmoothgroups.emplace_back();
                Utils::ParseIndex(args, esmoothgroups.back().key);
            }
            else if (!std::strncmp(c, "smoothangle", std::max(int(args-c), 11)))
            {
                if (esmoothgroups.back().flags & Utils::Esmoothgroup::F_USED) esmoothgroups.emplace_back();
                double angle = Utils::ParseAttrib(args, 0);
                esmoothgroups.back().angle = std::abs(std::cos(glm::clamp(angle, -180.0, 180.0) * M_PI/180));
            }
            else if (!std::strncmp(c, "smoothuv", std::max(int(args-c), 8)))
            {
                if (esmoothgroups.back().flags & Utils::Esmoothgroup::F_USED) esmoothgroups.emplace_back();
                int val = 1;
                if (Utils::ParseIndex(args, val) && val <= 0) esmoothgroups.back().flags &= ~Utils::Esmoothgroup::F_UVSMOOTH;
                else esmoothgroups.back().flags |= Utils::Esmoothgroup::F_UVSMOOTH;
            }
            else if (!std::strncmp(c, "mesh", std::max(int(args-c), 4)))
            {
                curmesh = Utils::TrimName(args);
                if (emeshes.empty() || emeshes.back().name != curmesh) needmesh = true;
                fmoffset = epositions.size();
#if 0
                emesh &m = emeshes.push_back();
                m.firsttri = etriangles.size();
                fmoffset = epositions.size();
                parsename(args, m.name);
#endif
            }
            else if (!std::strncmp(c, "material", std::max(int(args-c), 8)))
            {
                curmaterial = Utils::TrimName(args);
                if (emeshes.empty() || emeshes.back().material != curmaterial) needmesh = true;
                //            if(emeshes.size()) parsename(c, emeshes.back().material);
            }
            else if (!strncmp(c, "joint", std::max(int(args-c), 5)))
            {
                ejoints.emplace_back();
                Utils::Ejoint& j = ejoints.back();
                j.name = Utils::TrimName(args);
                Utils::ParseIndex(args, j.parent);
            }
            else if (!strncmp(c, "vertexarray", std::max(int(args-c), 11)))
            {
                evarrays.emplace_back();
                Utils::Evarray& va = evarrays.back();
                va.type = Utils::FindVarrayType(Utils::TrimName(args));
                va.format = Utils::FindVarrayFormat(Utils::TrimName(args));
                va.size = std::strtol(args, &args, 10);
                va.name = Utils::TrimName(args);
            }
            else if (!strncmp(c, "animation", std::max(int(args-c), 9)))
            {
                eanims.emplace_back();
                Utils::Eanim &a = eanims.back();
                a.name = Utils::TrimName(args);
                a.startframe = eframes.size();
                if (!eframes.size() || eframes.back() != (int)eposes.size()) eframes.push_back(eposes.size());
            }
            else if (!strncmp(c, "frame", std::max(int(args-c), 5)))
            {
                if (eanims.size() && eframes.size() && eframes.back() != (int)eposes.size()) eframes.push_back(eposes.size());
            }
            else if (!strncmp(c, "framerate", std::max(int(args-c), 9)))
            {
                if (eanims.size())
                {
                    double fps = Utils::ParseAttrib(args);
                    eanims.back().fps = std::max(fps, 0.0);
                }
            }
            else if (!strncmp(c, "loop", std::max(int(args-c), 4)))
            {
                if (eanims.size()) eanims.back().flags |= 1;//IQM_LOOP;
            }
            else if(!strncmp(c, "comment", std::max(int(args-c), 7)))
            {
                break;
                //if(commentdata.size()) break;
                //for(;;)
                //{
                //    int len = f->read(commentdata.reserve(1024), 1024);
                //    commentdata.advance(len);
                //    if(len < 1024) { commentdata.push_back('\0'); break; }
                //}
            }

        }

    }

}}
