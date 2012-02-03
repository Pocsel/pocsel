#ifndef __TOOLS_RECTANGLE_HPP__
#define __TOOLS_RECTANGLE_HPP__

#include "tools/Vector2.hpp"

namespace Tools {

    struct Rectangle
    {
    public:
        Vector2<int> pos;
        Vector2<unsigned int> size;

    public:
        Rectangle() : pos(), size() {}
        Rectangle(Vector2<int> pos, Vector2<unsigned int> size) : pos(pos), size(size) {}
        Rectangle(int x, int y, unsigned int w, unsigned int h) : pos(x, y), size(w, h) {}
        bool operator ==(Rectangle const& other) const
        {
            return this->pos == other.pos && this->size == other.size;
        }
        bool Contains(int x, int y) const
        {
            return
                this->pos.x < x &&
                x < this->pos.x + static_cast<int>(this->size.x) &&
                this->pos.y < y &&
                y < this->pos.y + static_cast<int>(this->size.y);
        }
        bool Contains(Vector2<int> const& vec) const
        {
            return this->Contains(vec.x, vec.y);
        }

        float GetRatio() const { return float(this->size.w) / float(this->size.h); }
    };

}

#endif
