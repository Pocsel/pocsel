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

    void Convert::_MakeAnims()
    {
        if (_escale != 1)
        {
            for (unsigned int i = 0; i < _eposes.size(); ++i)
            {
                _eposes[i].pos *= _escale;
            }
        }

        int numbasejoints = _eframes.size() ? _eframes[0] : _eposes.size();
        if (_meshes.size() && _joints.empty())
        {
            _mjoints.resize(0);
            for (unsigned int i = 0; i < _ejoints.size(); ++i)
            {
                Utils::Ejoint& ej = _ejoints[i];
                _joints.emplace_back();
                Iqm::Joint& j = _joints.back();
                j.name = _ShareString(ej.name);
                j.parent = ej.parent;
                if ((int)i < numbasejoints)
                {
                    //glm::mat3x3 mat33;//(_eposes[i].orient, _eposes[i].scale);

                    _mjoints.push_back(glm::mat3x3(glm::inverse(glm::mat4x4(
                                        glm::scale(_eposes[i].scale) *
                                        glm::translate(_eposes[i].pos) *
                                        glm::toMat4(_eposes[i].orient)
                                        ))));

                                        //glm::vec4(mat33[0], _eposes[i].pos.x),
                                        //glm::vec4(mat33[1], _eposes[i].pos.y),
                                        //glm::vec4(mat33[2], _eposes[i].pos.z),
                                        //glm::vec4(0)
                    //_mjoints.back() = glm::inverse(glm::mat3x4(glm::mat3x3(_eposes[i].orient, _eposes[i].scale), _eposes[i].pos));

                    for (unsigned int k = 0; k < 3; ++k)
                        j.position[k] = _eposes[i].pos[k];
                    for (unsigned int k = 0; k < 4; ++k)
                        j.orientation[k] = _eposes[i].orient[k];
                    for (unsigned int k = 0; k < 3; ++k)
                        j.size[k] = _eposes[i].scale[k];
                }
                else
                {
                    //glm::mat3x3 mat33;//glm::mat3x3(glm::inverse(glm::mat3x4(glm::quat(0, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)))));
                    _mjoints.push_back(glm::mat3x3(glm::inverse(glm::mat4x4(
                                        glm::scale(glm::vec3(1, 1, 1)) *
                                        glm::translate(glm::vec3(0, 0, 0)) *
                                        glm::toMat4(glm::quat(0, 0, 0, 1))
                                        //glm::vec4(mat33[0], _eposes[i].pos.x),
                                        //glm::vec4(mat33[1], _eposes[i].pos.y),
                                        //glm::vec4(mat33[2], _eposes[i].pos.z),
                                        //glm::vec4(0)
                                        ))));
                }
                if (ej.parent >= 0)
                    _mjoints[i] *= _mjoints[ej.parent];
            }
        }
        if (_eanims.empty())
            return;
        if (_poses.empty())
        {
            for (unsigned int i = 0; i < _ejoints.size(); ++i)
            {
                Utils::Ejoint& ej = _ejoints[i];
                _poses.emplace_back();
                Iqm::Pose &p = _poses.back();
                p.parent = ej.parent;
            }
        }
        if (_poses.empty())
            return;
        int totalframes = _frames.size() / _poses.size();
        //glm::mat3x4 *mbuf = _mpositions.size() && _mblends.size() && _mjoints.size() ? new glm::mat3x4[_poses.size()] : 0;
        for (unsigned int i = 0; i < _eanims.size(); ++i)
        {
            Utils::Eanim& ea = _eanims[i];
            _anims.emplace_back();
            Iqm::Anim &a = _anims.back();
            a.name = _ShareString(ea.name);
            a.first_frame = totalframes;
            a.num_frames = 0;
            a.framerate = ea.fps;
            a.flags = ea.flags;
            for (int j = ea.startframe, end = (i+1 < _eanims.size()) ? _eanims[i+1].startframe : _eframes.size(); j < end && j <= ea.endframe; ++j)
            {
                int offset = _eframes[j];
                int range = (j+1<(int)_eframes.size() ? _eframes[j+1] : _eposes.size()) - offset;
                if (range <= 0)
                    continue;
                for (int k = 0; k < std::min(range, (int)_poses.size()); ++k)
                    _frames.push_back(_eposes[offset + k]);
                for (int k = 0; k < std::max((int)_poses.size() - (int)range, 0); ++k)
                    _frames.push_back(Transform(glm::vec3(0, 0, 0), glm::quat(0, 0, 0, 1), glm::vec3(1, 1, 1)));
                //if (mbuf)
                //    _MakeBounds(_bounds.push_back(), mbuf, mjoints.getbuf(), &frames[frames.size() - poses.size()]);
                a.num_frames++;
            }
            totalframes += a.num_frames;
        }
        //if(mbuf) delete[] mbuf;
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

    void Convert::_MakeNeighbors()
    {
        std::map<Utils::NeighborKey, Utils::NeighborVal> nhash;

        for (unsigned int i = 0; i < _triangles.size(); ++i)
        {
            Iqm::Triangle& t = _triangles[i];
            for (int j = 0, p = 2; j < 3; p = j++)
            {
                Utils::NeighborKey key(t.vertex[p], t.vertex[j]);
                if (nhash.count(key))
                    nhash[key].Add(i);
                else
                    nhash[key] = Utils::NeighborVal(i);
            }
        }

        for (unsigned int i = 0; i < _triangles.size(); ++i)
        {
            Iqm::Triangle& t = _triangles[i];
            _neighbors.emplace_back();
            Iqm::Triangle& n = _neighbors.back();
            for(int j = 0, p = 2; j < 3; p = j, j++)
                n.vertex[p] = nhash[Utils::NeighborKey(t.vertex[p], t.vertex[j])].Opposite(i);
        }
    }

    void Convert::_MakeTriangles(std::vector<Utils::TriangleInfo>& tris, std::vector<Utils::SharedVert> const& mmap)
    {
        Utils::TriangleInfo** uses = new Utils::TriangleInfo*[3*tris.size()];
        Utils::VertexCache *verts = new Utils::VertexCache[mmap.size()];
        std::list<Utils::VertexCache*> vcache;

        //std::vector<std::vector<Utils::TriangleInfo>> uses(3*tris.size());
        //std::list<Utils::VertexCache> vcache;

        for (unsigned int i = 0; i < tris.size(); ++i)
        {
            Utils::TriangleInfo& t = tris[i];
            t.used = t.vert[0] == t.vert[1] || t.vert[1] == t.vert[2] || t.vert[2] == t.vert[0];
            if (t.used)
                continue;
            for (unsigned int k = 0; k < 3; ++k)
                verts[t.vert[k]].numuses++;
        }
        Utils::TriangleInfo **curuse = uses;
        for (int i = tris.size() - 1; i >= 0; --i)
        {
            Utils::TriangleInfo &t = tris[i];
            if (t.used)
                continue;
            for (unsigned int k = 0; k < 3; ++k)
            {
                Utils::VertexCache &v = verts[t.vert[k]];
                if (!v.uses)
                {
                    curuse += v.numuses;
                    v.uses = curuse;
                }
                *--v.uses = &t;
            }
        }
        for (unsigned int i = 0; i < mmap.size(); ++i)
            verts[i].CalcScore();
        Utils::TriangleInfo* besttri = 0;
        float bestscore = -1e16f;
        for (unsigned int i = 0; i < tris.size(); ++i)
        {
            Utils::TriangleInfo &t = tris[i];
            if (t.used)
                continue;
            t.score = verts[t.vert[0]].score + verts[t.vert[1]].score + verts[t.vert[2]].score;
            if (t.score > bestscore)
            {
                besttri = &t;
                bestscore = t.score;
            }
        }

        //int reloads = 0, n = 0;
        while (besttri)
        {
            besttri->used = true;
            _triangles.emplace_back();
            Iqm::Triangle& t = _triangles.back();
            for (unsigned int k = 0; k < 3; ++k)
            {
                Utils::VertexCache& v = verts[besttri->vert[k]];
                if (v.index < 0)
                {
                    v.index = _vmap.size();
                    _vmap.push_back(mmap[besttri->vert[k]]);
                }
                t.vertex[k] = v.index;
                v.RemoveUse(besttri);
                if (v.rank >= 0)
                {
                    for (auto it = vcache.begin(), ite = vcache.end(); it != ite; ++it)
                    {
                        if (*it == &v)
                        {
                            vcache.erase(it);
                            v.rank = -1;
                            break;
                        }
                    }
                }
                if (v.numuses <= 0)
                    continue;
                vcache.push_front(&v);
                v.rank = 0;
            }
            int rank = 0;
            for (auto it = vcache.begin(), ite = vcache.end(); it != ite; ++it)
            {
                Utils::VertexCache* v = *it;
                v->rank = rank++;
                v->CalcScore();
            }
            besttri = 0;
            bestscore = -1e16f;
            for (auto it = vcache.begin(), ite = vcache.end(); it != ite; ++it)
            {
                Utils::VertexCache* v = *it;
                for (int i = 0; i < v->numuses; ++i)
                {
                    Utils::TriangleInfo& t = *v->uses[i];
                    t.score = verts[t.vert[0]].score + verts[t.vert[1]].score + verts[t.vert[2]].score;
                    if (t.score > bestscore)
                    {
                        besttri = &t;
                        bestscore = t.score;
                    }
                }
            }
            while (vcache.size() > Utils::VertexCache::MaxVcache)
            {
                vcache.back()->rank = -1;
                vcache.pop_back();
            }
            if (!besttri)
            {
                for (unsigned int i = 0; i < tris.size(); ++i)
                {
                    Utils::TriangleInfo &t = tris[i];
                    if (!t.used && t.score > bestscore)
                    {
                        besttri = &t;
                        bestscore = t.score;
                    }
                }
            }
        }

        Tools::Delete(uses);
        Tools::Delete(verts);
    }

    void Convert::_MakeMeshes()
    {
        _meshes.resize(0);
        _triangles.resize(0);
        _neighbors.resize(0);
        _vmap.resize(0);
        _varrays.resize(0);
        _vdata.resize(0);

        std::map<Utils::SharedVert, unsigned int> mshare;
        std::vector<Utils::SharedVert> mmap;
        std::vector<Utils::TriangleInfo> tinfo;

        for (unsigned int i = 0; i < _emeshes.size(); ++i)
        {
            Utils::Emesh& em1 = _emeshes[i];
            if (em1.used)
                continue;
            for (unsigned int j = i; j < _emeshes.size(); ++j)
            {
                Utils::Emesh& em = _emeshes[j];
                if (em.name != em1.name || em.material != em1.material)
                    continue;
                int lasttri = (i+1<_emeshes.size()) ? _emeshes[i+1].firsttri : _etriangles.size();
                for(int k = em.firsttri; k < lasttri; ++k)
                {
                    Utils::Etriangle& et = _etriangles[k];
                    tinfo.emplace_back();
                    Utils::TriangleInfo& t = tinfo.back();
                    for (unsigned int l = 0; l < 3; ++l)
                    {
                        Utils::SharedVert v(et.vert[l], et.weld[l]);
                        if (mshare.count(v))
                            t.vert[l] = mshare[v];
                        else
                        {
                            mshare[v] = mmap.size();
                            t.vert[l] = mshare[v];
                        }
                        if (!(t.vert[l] < mmap.size()))
                            mmap.push_back(v);
                    }
                }
                em.used = true;
            }
            if(tinfo.empty()) continue;
            _meshes.emplace_back();
            Iqm::Mesh &m = _meshes.back();
            m.name = _ShareString(em1.name);
            m.material = _ShareString(em1.material);
            m.first_triangle = _triangles.size();
            m.first_vertex = _vmap.size();
            _MakeTriangles(tinfo, mmap);
            m.num_triangles = _triangles.size() - m.first_triangle;
            m.num_vertexes = _vmap.size() - m.first_vertex;

            mshare.clear();
            mmap.resize(0);
            tinfo.resize(0);
        }

        if (_triangles.size())
            _MakeNeighbors();

        if (_escale != 1)
        {
            for (auto it = _epositions.begin(), ite = _epositions.end(); it != ite; ++it)
                *it *= _escale;
        }
        if (_epositions.size())
            _SetupVertexArray<Iqm::VertexArrayType::Position, Iqm::VertexArrayFormat::Float>(_epositions, 3);
        if (_etexcoords.size())
            _SetupVertexArray<Iqm::VertexArrayType::Texcoord, Iqm::VertexArrayFormat::Float>(_etexcoords, 2);
        if (_enormals.size())
            _SetupVertexArray<Iqm::VertexArrayType::Normal, Iqm::VertexArrayFormat::Float>(_enormals, 3);
        //if (_etangents.size())
        //{
        //    if (_ebitangents.size() && _enormals.size())
        //    {
        //        for (unsigned int i = 0; i < _etangents.size(); ++i)
        //        {
        //            if (i < _ebitangents.size() && i < _enormals.size())
        //                _etangents[i].w = glm::dot(glm::cross(_enormals[i], glm::dvec3(_etangents[i])), (_ebitangents[i])) < 0 ? -1 : 1;
        //        }
        //    }
        //    _SetupVertexArray<Iqm::VertexArrayType::Tangent, Iqm::VertexArrayFormat::Float>(_etangents, 4);
        //}
        //else if (_enormals.size() && _etexcoords.size())
        //{
        //    _CalcTangents();
        //    _SetupVertexArray<Iqm::VertexArrayType::Tangent, Iqm::VertexArrayFormat::Float>(_etangents, 4);
        //}
        if (_eblends.size())
        {
            _SetupVertexArray<Iqm::VertexArrayType::BlendIndexes, Iqm::VertexArrayFormat::Ubyte>(_eblends, 4);
            _SetupVertexArray<Iqm::VertexArrayType::BlendWeights, Iqm::VertexArrayFormat::Ubyte>(_eblends, 4);
        }
        //if (_ecolors.size())
        //    _SetupVertexArray<Iqm::VertexArrayType::Color, Iqm::VertexArrayFormat::Ubyte>(_ecolors, 4);
        //loopi(10) if(ecustom[i].size()) setupvertexarray<IQM_CUSTOM>(ecustom[i], IQM_CUSTOM + i, IQM_FLOAT, 4);

        if (_epositions.size())
        {
            _mpositions.resize(0);
            _mpositions.swap(_epositions);
        }
        if (_eblends.size())
        {
            _mblends.resize(0);
            _mblends.swap(_eblends);
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
                            //glm::dvec4 tangent(Utils::ParseAttribs3(c), 0);
                            //glm::dvec3 bitangent(0, 0, 0);
                            //bitangent.x = Utils::ParseAttrib(c);
                            //if (Utils::MaybeParseAttrib(c, bitangent.y))
                            //{
                            //    bitangent.z = Utils::ParseAttrib(c);
                            //    _ebitangents.push_back(bitangent);
                            //}
                            //else
                            //    tangent.w = bitangent.x;
                            //_etangents.push_back(tangent);
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
                                if (t.orient.w > 0)
                                    t.orient *= -1;
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
                                if (t.orient.w > 0)
                                    t.orient *= -1;
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
                                if (t.orient.w > 0)
                                    t.orient *= -1;
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
        _MakeMeshes();
        _MakeAnims();

        return true;
    }

    bool Convert::_WriteMqm(std::string const& file)
    {

    }

}}
