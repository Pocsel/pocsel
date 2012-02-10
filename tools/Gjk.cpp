#include "tools/Gjk.hpp"

namespace Tools {

    boost::thread_specific_ptr<Gjk> Gjk::_instance;
    int Gjk::_bitsToIndices[] =
    {
        0,
        1,
        2,
        17,
        3,
        25,
        26,
        209,
        4,
        33,
        34,
        273,
        35,
        281,
        282,
        2257
    };

    Gjk::Gjk()
    {
    }

    Gjk::~Gjk()
    {
    }

    bool Gjk::AddSupportPoint(Vector3d const& newPoint)
    {
        int num = (Gjk::_bitsToIndices[this->_simplexBits ^ 15] & 7) - 1;
        this->_y[num] = newPoint;
        this->_yLengthSq[num] = newPoint.GetMagnitudeSquared();
        for (int num2 = Gjk::_bitsToIndices[this->_simplexBits]; num2 != 0; num2 >>= 3)
        {
            int num3 = (num2 & 7) - 1;
            Vector3d vector = this->_y[num3] - newPoint;
            this->_edges[num3][num] = vector;
            this->_edges[num][num3] = -vector;
            this->_edgeLengthSq[num][num3] = (this->_edgeLengthSq[num3][num] = vector.GetMagnitudeSquared());
        }
        this->_UpdateDeterminant(num);
        return this->_UpdateSimplex(num);
    }

    void Gjk::Reset()
    {
        this->_simplexBits = 0;
        this->_maxLengthSq = 0.0f;
    }

    void Gjk::_UpdateDeterminant(int xmIdx)
    {
        int num = 1 << xmIdx;
        this->_det[num][xmIdx] = 1.0f;
        int num2 = Gjk::_bitsToIndices[this->_simplexBits];
        int num3 = num2;
        int num4 = 0;
        while (num3 != 0)
        {
            int num5 = (num3 & 7) - 1;
            int num6 = 1 << num5;
            int num7 = num6 | num;
            this->_det[num7][num5] = Vector3d::Dot(this->_edges[xmIdx][num5], this->_y[xmIdx]);
            this->_det[num7][xmIdx] = Vector3d::Dot(this->_edges[num5][xmIdx], this->_y[num5]);
            int num8 = num2;
            for (int i = 0; i < num4; i++)
            {
                int num9 = (num8 & 7) - 1;
                int num10 = 1 << num9;
                int num11 = num7 | num10;
                int num12 = (this->_edgeLengthSq[num5][num9] < this->_edgeLengthSq[xmIdx][num9]) ? num5 : xmIdx;
                this->_det[num11][num9] = this->_det[num7][num5] * Vector3d::Dot(this->_edges[num12][num9], this->_y[num5]) + this->_det[num7][xmIdx] * Vector3d::Dot(this->_edges[num12][num9], this->_y[xmIdx]);
                num12 = ((this->_edgeLengthSq[num9][num5] < this->_edgeLengthSq[xmIdx][num5]) ? num9 : xmIdx);
                this->_det[num11][num5] = this->_det[num10 | num][num9] * Vector3d::Dot(this->_edges[num12][num5], this->_y[num9]) + this->_det[num10 | num][xmIdx] * Vector3d::Dot(this->_edges[num12][num5], this->_y[xmIdx]);
                num12 = ((this->_edgeLengthSq[num5][xmIdx] < this->_edgeLengthSq[num9][xmIdx]) ? num5 : num9);
                this->_det[num11][xmIdx] = this->_det[num6 | num10][num9] * Vector3d::Dot(this->_edges[num12][xmIdx], this->_y[num9]) + this->_det[num6 | num10][num5] * Vector3d::Dot(this->_edges[num12][xmIdx], this->_y[num5]);
                num8 >>= 3;
            }
            num3 >>= 3;
            num4++;
        }
        if ((this->_simplexBits | num) == 15)
        {
            int num13 = (this->_edgeLengthSq[1][0] < this->_edgeLengthSq[2][0]) ? ((this->_edgeLengthSq[1][0] < this->_edgeLengthSq[3][0]) ? 1 : 3) : ((this->_edgeLengthSq[2][0] < this->_edgeLengthSq[3][0]) ? 2 : 3);
            this->_det[15][0] = this->_det[14][1] * Vector3d::Dot(this->_edges[num13][0], this->_y[1]) + this->_det[14][2] * Vector3d::Dot(this->_edges[num13][0], this->_y[2]) + this->_det[14][3] * Vector3d::Dot(this->_edges[num13][0], this->_y[3]);
            num13 = ((this->_edgeLengthSq[0][1] < this->_edgeLengthSq[2][1]) ? ((this->_edgeLengthSq[0][1] < this->_edgeLengthSq[3][1]) ? 0 : 3) : ((this->_edgeLengthSq[2][1] < this->_edgeLengthSq[3][1]) ? 2 : 3));
            this->_det[15][1] = this->_det[13][0] * Vector3d::Dot(this->_edges[num13][1], this->_y[0]) + this->_det[13][2] * Vector3d::Dot(this->_edges[num13][1], this->_y[2]) + this->_det[13][3] * Vector3d::Dot(this->_edges[num13][1], this->_y[3]);
            num13 = ((this->_edgeLengthSq[0][2] < this->_edgeLengthSq[1][2]) ? ((this->_edgeLengthSq[0][2] < this->_edgeLengthSq[3][2]) ? 0 : 3) : ((this->_edgeLengthSq[1][2] < this->_edgeLengthSq[3][2]) ? 1 : 3));
            this->_det[15][2] = this->_det[11][0] * Vector3d::Dot(this->_edges[num13][2], this->_y[0]) + this->_det[11][1] * Vector3d::Dot(this->_edges[num13][2], this->_y[1]) + this->_det[11][3] * Vector3d::Dot(this->_edges[num13][2], this->_y[3]);
            num13 = ((this->_edgeLengthSq[0][3] < this->_edgeLengthSq[1][3]) ? ((this->_edgeLengthSq[0][3] < this->_edgeLengthSq[2][3]) ? 0 : 2) : ((this->_edgeLengthSq[1][3] < this->_edgeLengthSq[2][3]) ? 1 : 2));
            this->_det[15][3] = this->_det[7][0] * Vector3d::Dot(this->_edges[num13][3], this->_y[0]) + this->_det[7][1] * Vector3d::Dot(this->_edges[num13][3], this->_y[1]) + this->_det[7][2] * Vector3d::Dot(this->_edges[num13][3], this->_y[2]);
        }
    }

    bool Gjk::_UpdateSimplex(int newIndex)
    {
        int num = this->_simplexBits | 1 << newIndex;
        int num2 = 1 << newIndex;
        for (int num3 = this->_simplexBits; num3 != 0; num3--)
        {
            if ((num3 & num) == num3 && this->_IsSatisfiesRule(num3 | num2, num))
            {
                this->_simplexBits = (num3 | num2);
                this->_closestPoint = this->_ComputeClosestPoint();
                return true;
            }
        }
        bool result = false;
        if (this->_IsSatisfiesRule(num2, num))
        {
            this->_simplexBits = num2;
            this->_closestPoint = this->_y[newIndex];
            this->_maxLengthSq = this->_yLengthSq[newIndex];
            result = true;
        }
        return result;
    }

    Vector3d Gjk::_ComputeClosestPoint()
    {
        double num = 0.0f;
        Vector3d vector(0.0f);
        this->_maxLengthSq = 0.0f;
        for (int num2 = Gjk::_bitsToIndices[this->_simplexBits]; num2 != 0; num2 >>= 3)
        {
            int num3 = (num2 & 7) - 1;
            double num4 = this->_det[this->_simplexBits][num3];
            num += num4;
            vector += this->_y[num3] * num4;
            this->_maxLengthSq = std::max(this->_maxLengthSq, this->_yLengthSq[num3]);
        }
        return vector / num;
    }

    bool Gjk::_IsSatisfiesRule(int xBits, int yBits)
    {
        bool result = true;
        for (int num = Gjk::_bitsToIndices[yBits]; num != 0; num >>= 3)
        {
            int num2 = (num & 7) - 1;
            int num3 = 1 << num2;
            if ((num3 & xBits) != 0)
            {
                if (this->_det[xBits][num2] <= 0.0f)
                {
                    result = false;
                    break;
                }
            }
            else
            {
                if (this->_det[xBits | num3][num2] > 0.0f)
                {
                    result = false;
                    break;
                }
            }
        }
        return result;
    }

}
