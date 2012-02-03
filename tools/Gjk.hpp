#ifndef __TOOLS_GJK_HPP__
#define __TOOLS_GJK_HPP__

#include <boost/thread/tss.hpp>

#include "tools/Vector3.hpp"

namespace Tools {

    // Gilbert Johnson Keerthi distance algorithm
    class Gjk
    {
    private:
        static boost::thread_specific_ptr<Gjk> _instance;
        static int _bitsToIndices[];

        Vector3d _closestPoint;
        Vector3d _y[4];
        double _yLengthSq[4];
        Vector3d _edges[4][4];
        double _edgeLengthSq[4][4];
        double _det[16][4];
        int _simplexBits;
        double _maxLengthSq;

    public:
        static Gjk& GetInstance()
        {
            if (Gjk::_instance.get() == static_cast<Gjk*>(0))
                Gjk::_instance.reset(new Gjk());
            return *Gjk::_instance;
        }

        ~Gjk();

        bool IsFullSimplex() const { return this->_simplexBits == 15; }
        double GetMaxLengthSquared() const { return this->_maxLengthSq; }
        Vector3d const& GetClosestPoint() const { return this->_closestPoint; }
        bool AddSupportPoint(Vector3d const& newPoint);
        void Reset();

    private:
        Gjk();

        void _UpdateDeterminant(int xmIdx);
        bool _UpdateSimplex(int newIndex);
        Vector3d _ComputeClosestPoint();
        bool _IsSatisfiesRule(int xBits, int yBits);
    };

}

#endif
