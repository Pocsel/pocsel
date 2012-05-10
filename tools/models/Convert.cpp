#include "tools/models/Convert.hpp"
#include "tools/models/Utils.hpp"
#include "tools/Math.hpp"

#include "glm/std_utils.hpp"

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

    void Convert::_WeldVert(std::vector<glm::vec3> const& norms, WeldInfo* welds, int& numwelds, Utils::UnionFind<int>& welder)
    {
        welder.Clear();
        int windex = 0;
        for (WeldInfo* w = welds; w; w = w->next, windex++)
        {
            Utils::Etriangle& wt = _etriangles[w->tri];
            Utils::Esmoothgroup& wg = _esmoothgroups[wt.smoothgroup];
            int vindex = windex + 1;
            for (WeldInfo *v = w->next; v; v = v->next, vindex++)
            {
                Utils::Etriangle &vt = _etriangles[v->tri];
                Utils::Esmoothgroup &vg = _esmoothgroups[vt.smoothgroup];
                if ((wg.key != vg.key)
                        ||
                        (glm::dot(norms[w->tri], norms[v->tri]) < std::max(wg.angle, vg.angle))
                        ||
                        (((wg.flags | vg.flags) & Utils::Esmoothgroup::F_UVSMOOTH) &&
                         _etexcoords[wt.vert[w->vert]] != _etexcoords[vt.vert[v->vert]])
                        ||
                        ((int)_esmoothindexes.size() > std::max(w->vert, v->vert) && _esmoothindexes[w->vert] != _esmoothindexes[v->vert]))
                    continue;
                if (_esmoothedges.size())
                {
                    int w0 = w->vert, w1 = (w->vert+1)%3, w2 = (w->vert+2)%3;
                    glm::dvec4 const& wp1 = _epositions[wt.vert[w1]],
                          &wp2 = _epositions[wt.vert[w2]];
                    int v0 = v->vert, v1 = (v->vert+1)%3, v2 = (v->vert+2)%3;
                    glm::dvec4 const& vp1 = _epositions[vt.vert[v1]],
                          &vp2 = _epositions[vt.vert[v2]];
                    int wf = _esmoothedges[w->tri], vf = _esmoothedges[v->tri];
                    if ((wp1 != vp1 || !(((wf>>w0)|(vf>>v0))&1)) &&
                            (wp1 != vp2 || !(((wf>>w0)|(vf>>v2))&1)) &&
                            (wp2 != vp1 || !(((wf>>w2)|(vf>>v0))&1)) &&
                            (wp2 != vp2 || !(((wf>>w2)|(vf>>v2))&1)))
                        continue;
                }
                welder.Unite(windex, vindex, -1);
            }
        }
        windex = 0;
        for (WeldInfo *w = welds; w; w = w->next, windex++)
        {
            Utils::Etriangle &wt = _etriangles[w->tri];
            wt.weld[w->vert] = welder.Find(windex, -1, numwelds);
            if (wt.weld[w->vert] == Uint32(numwelds)) numwelds++;
        }
    }

    void Convert::_SmoothVerts(bool areaweight)
    {
        if (_etriangles.empty()) return;

        if (_enormals.size())
        {
            for (auto it = _etriangles.begin(), ite = _etriangles.end(); it != ite; ++it)
            {
                Utils::Etriangle &t = *it;
                for (unsigned int k = 0; k < 3; ++k)
                    t.weld[k] = t.vert[k];
            }
            return;
        }

        if (_etexcoords.empty())
        {
            for (auto it = _esmoothgroups.begin(), ite = _esmoothgroups.end(); it != ite; ++it)
                it->flags &= ~Utils::Esmoothgroup::F_UVSMOOTH;
        }

        if (_esmoothedges.size())
        {
            while (_esmoothedges.size() < _etriangles.size())
                _esmoothedges.push_back(7);
        }

        std::vector<glm::vec3> tarea, tnorms;
        for (auto it = _etriangles.begin(), ite = _etriangles.end(); it != ite; ++it)
        {
            Utils::Etriangle &t = *it;
            glm::vec3 v0(_epositions[t.vert[0]]);
            glm::vec3 v1(_epositions[t.vert[1]]);
            glm::vec3 v2(_epositions[t.vert[2]]);
            glm::vec3 cross = glm::cross(v2 - v0, v1 - v0);
            tarea.push_back(cross);
            tnorms.push_back(glm::normalize(cross));
        }

        int nextalloc = 0;
        std::vector<std::unique_ptr<WeldInfo>> allocs;
        std::map<glm::dvec4, WeldInfo*> welds;

        for (unsigned int i = 0; i < _etriangles.size(); ++i)
        {
            Utils::Etriangle &t = _etriangles[i];
            for (unsigned int k = 0; k < 3; ++k)
            {
                WeldInfo** next;
                if (welds.count(_epositions[t.vert[k]]))
                    next = &welds[_epositions[t.vert[k]]];
                else
                {
                    next = &welds[_epositions[t.vert[k]]];
                    next = 0;
                }

                if (!(nextalloc % 4096))
                    allocs.emplace_back(new WeldInfo[4096]);
                WeldInfo& w = allocs[nextalloc/4096].get()[nextalloc%4096];
                ++nextalloc;
                w.tri = i;
                w.vert = k;
                w.next = *next;
                *next = &w;
            }
        }

        int numwelds = 0;
        Utils::UnionFind<int> welder;

        for (auto it = welds.begin(), ite = welds.end(); it != ite; ++it)
            _WeldVert(tnorms, it->second, numwelds, welder);

        for (int i = 0; i < numwelds; ++i)
            _enormals.push_back(glm::dvec3(0, 0, 0));

        for (unsigned int i = 0; i < _etriangles.size(); ++i)
        {
            Utils::Etriangle &t = _etriangles[i];
            for (unsigned int k = 0; k < 3; ++k)
                _enormals[t.weld[k]] += areaweight ? tarea[i] : tnorms[i];
        }

        for (unsigned int i = 0; i < _enormals.size(); ++i)
        {
            if (_enormals[i] != glm::dvec3(0, 0, 0))
                _enormals[i] = glm::normalize(_enormals[i]);
        }
    }

    bool Convert::_LoadIqe(std::string const& path)
    {
        std::ifstream f(path);
        if (!f)
        {
            Tools::error << "could not open " << path << "\n";
            return false;
        }

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
                        _epositions.push_back(Utils::ParseAttribs4(c, glm::dvec4(0, 0, 0, 1))); continue;
                    case 't':
                        _etexcoords.push_back(Utils::ParseAttribs4(c)); continue;
                    case 'n':
                        _enormals.push_back(Utils::ParseAttribs3(c)); continue;
                    case 'x':
                        {
                            glm::dvec4 tangent(Utils::ParseAttribs3(c), 0);
                            glm::dvec3 bitangent(0, 0, 0);
                            bitangent.x = Utils::ParseAttrib(c);
                            if (Utils::MaybeParseAttrib(c, bitangent.y))
                            {
                                bitangent.z = Utils::ParseAttrib(c);
                                _ebitangents.push_back(bitangent);
                            }
                            else
                                tangent.w = bitangent.x;
                            _etangents.push_back(tangent);
                            continue;
                        }
                    case 'b':
                        _eblends.push_back(Utils::ParseBlends(c)); continue;
                    case 'c':
                        continue; //ecolors.push_back(parseattribs4(c, Vec4(0, 0, 0, 1))); continue;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        //ecustom[c[-1] - '0'].push_back(parseattribs4(c));
                        continue;
                    case 's':
                        int idx;
                        Utils::ParseIndex(c, idx);
                        _esmoothindexes.push_back(idx);
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
                                _eposes.push_back(t);
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
                                _eposes.push_back(t);
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
                                _eposes.push_back(t);
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
                                _emeshes.push_back(Utils::Emesh(curmesh, curmaterial, _etriangles.size()));
                                needmesh = false;
                            }
                            if (i1 < 0) i1 = std::max<int>(_epositions.size() + i1, 0);
                            if (i2 < 0) i2 = std::max<int>(_epositions.size() + i2, 0);
                            while (Utils::ParseIndex(c, i3))
                            {
                                if (i3 < 0) i3 = std::max<int>(_epositions.size() + i3, 0);
                                _esmoothgroups.back().flags |= Utils::Esmoothgroup::F_USED;
                                _etriangles.push_back(Utils::Etriangle(i1, i2, i3, _esmoothgroups.size()-1));
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
                                _emeshes.push_back(Utils::Emesh(curmesh, curmaterial, _etriangles.size()));
                                needmesh = false;
                            }
                            i1 = i1 < 0 ? std::max<int>(_epositions.size() + i1, 0) : (fmoffset + i1);
                            i2 = i2 < 0 ? std::max<int>(_epositions.size() + i2, 0) : (fmoffset + i2);
                            while (Utils::ParseIndex(c, i3))
                            {
                                i3 = i3 < 0 ? std::max<int>(_epositions.size() + i3, 0) : (fmoffset + i3);
                                _esmoothgroups.back().flags |= Utils::Esmoothgroup::F_USED;
                                _etriangles.push_back(Utils::Etriangle(i1, i2, i3, _esmoothgroups.size()-1));
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
                            _esmoothgroups.back().flags |= Utils::Esmoothgroup::F_USED;
                            while (Utils::ParseIndex(c, i3))
                            {
                                _esmoothedges.push_back(flags | 4);
                                flags = 1 | ((flags & 4) >> 1) | (glm::clamp(i3, 0, 1)<<2);
                            }
                            _esmoothedges.push_back(flags);
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
                if (_esmoothgroups.back().flags & Utils::Esmoothgroup::F_USED) _esmoothgroups.emplace_back();
                Utils::ParseIndex(args, _esmoothgroups.back().key);
            }
            else if (!std::strncmp(c, "smoothangle", std::max(int(args-c), 11)))
            {
                if (_esmoothgroups.back().flags & Utils::Esmoothgroup::F_USED) _esmoothgroups.emplace_back();
                double angle = Utils::ParseAttrib(args, 0);
                _esmoothgroups.back().angle = std::abs(std::cos(glm::clamp(angle, -180.0, 180.0) * M_PI/180));
            }
            else if (!std::strncmp(c, "smoothuv", std::max(int(args-c), 8)))
            {
                if (_esmoothgroups.back().flags & Utils::Esmoothgroup::F_USED) _esmoothgroups.emplace_back();
                int val = 1;
                if (Utils::ParseIndex(args, val) && val <= 0) _esmoothgroups.back().flags &= ~Utils::Esmoothgroup::F_UVSMOOTH;
                else _esmoothgroups.back().flags |= Utils::Esmoothgroup::F_UVSMOOTH;
            }
            else if (!std::strncmp(c, "mesh", std::max(int(args-c), 4)))
            {
                curmesh = Utils::TrimName(args);
                if (_emeshes.empty() || _emeshes.back().name != curmesh) needmesh = true;
                fmoffset = _epositions.size();
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
                if (_emeshes.empty() || _emeshes.back().material != curmaterial) needmesh = true;
                //            if(emeshes.size()) parsename(c, emeshes.back().material);
            }
            else if (!strncmp(c, "joint", std::max(int(args-c), 5)))
            {
                _ejoints.emplace_back();
                Utils::Ejoint& j = _ejoints.back();
                j.name = Utils::TrimName(args);
                Utils::ParseIndex(args, j.parent);
            }
            else if (!strncmp(c, "vertexarray", std::max(int(args-c), 11)))
            {
                _evarrays.emplace_back();
                Utils::Evarray& va = _evarrays.back();
                va.type = Utils::FindVarrayType(Utils::TrimName(args));
                va.format = Utils::FindVarrayFormat(Utils::TrimName(args));
                va.size = std::strtol(args, &args, 10);
                va.name = Utils::TrimName(args);
            }
            else if (!strncmp(c, "animation", std::max(int(args-c), 9)))
            {
                _eanims.emplace_back();
                Utils::Eanim &a = _eanims.back();
                a.name = Utils::TrimName(args);
                a.startframe = _eframes.size();
                if (!_eframes.size() || _eframes.back() != (int)_eposes.size()) _eframes.push_back(_eposes.size());
            }
            else if (!strncmp(c, "frame", std::max(int(args-c), 5)))
            {
                if (_eanims.size() && _eframes.size() && _eframes.back() != (int)_eposes.size()) _eframes.push_back(_eposes.size());
            }
            else if (!strncmp(c, "framerate", std::max(int(args-c), 9)))
            {
                if (_eanims.size())
                {
                    double fps = Utils::ParseAttrib(args);
                    _eanims.back().fps = std::max(fps, 0.0);
                }
            }
            else if (!strncmp(c, "loop", std::max(int(args-c), 4)))
            {
                //if (eanims.size()) eanims.back().flags |= 1;//IQM_LOOP;
            }
            else if(!strncmp(c, "comment", std::max(int(args-c), 7)))
            {
                //if(commentdata.size()) break;
                //for(;;)
                //{
                //    int len = f->read(commentdata.reserve(1024), 1024);
                //    commentdata.advance(len);
                //    if(len < 1024) { commentdata.push_back('\0'); break; }
                //}
            }

        }

        _SmoothVerts();
        //makemeshes();
        //makeanims();

        return true;
    }

    bool Convert::_WriteMqm(std::string const& file)
    {

    }

}}
