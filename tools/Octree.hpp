#ifndef __TOOLS_OCTREE_HPP__
#define __TOOLS_OCTREE_HPP__

#include "tools/AlignedCube.hpp"
//#include "tools/MemoryPool.hpp"

namespace Tools {

    template<class T>
    class Octree
        : public AlignedCube,
        private boost::noncopyable
    {
    private:
        union
        {
            Octree<T>* _childs[8];
            T* _elements[8];
        };
        unsigned int _size;
        int _count;

    public:
        Octree(Vector3d const& position, unsigned int size)
            : AlignedCube(position, size),
            _size(size),
            _count(0)
        {
            std::memset(this->_childs, 0, sizeof(this->_childs));
            std::memset(this->_elements, 0, sizeof(this->_elements));
        }
        ~Octree()
        {
            if (this->_count > 8)
                for (int i = 0; i < 8; ++i)
                    Tools::Delete(this->_childs[i]);
        }

        void InsertElement(T* element)
        {
            if (this->_count < 8)
                this->_elements[this->_count] = element;
            else if (this->_count == 8)
            {
                T* elements[8];
                std::memcpy(elements, this->_elements, sizeof(this->_elements));
                unsigned int size = this->_size / 2;
                this->_childs[0] = new Octree<T>(this->GetPosition() + Vector3d(   0,    0,    0), size);
                this->_childs[1] = new Octree<T>(this->GetPosition() + Vector3d(   0,    0, size), size);
                this->_childs[2] = new Octree<T>(this->GetPosition() + Vector3d(   0, size, 0   ), size);
                this->_childs[3] = new Octree<T>(this->GetPosition() + Vector3d(   0, size, size), size);
                this->_childs[4] = new Octree<T>(this->GetPosition() + Vector3d(size,    0, 0   ), size);
                this->_childs[5] = new Octree<T>(this->GetPosition() + Vector3d(size,    0, size), size);
                this->_childs[6] = new Octree<T>(this->GetPosition() + Vector3d(size, size, 0   ), size);
                this->_childs[7] = new Octree<T>(this->GetPosition() + Vector3d(size, size, size), size);

                for (int i = 0; i < 8; ++i)
                {
                    if (this->_childs[i]->Contains(*element) == AbstractCollider::Inside)
                        this->_childs[i]->InsertElement(element);
                    for (int j = 0; j < 8; ++j)
                        if (this->_childs[i]->Contains(*elements[j]) == AbstractCollider::Inside)
                            this->_childs[i]->InsertElement(elements[j]);
                }
            }
            else
            {
                for (int i = 0; i < 8; ++i)
                    if (this->_childs[i]->Contains(*element) == AbstractCollider::Inside)
                        this->_childs[i]->InsertElement(element);
            }
            this->_count++;
        }

        bool RemoveElement(T* element)
        {
            if (this->_count == 0)
                return false;
            if (this->_count <= 8)
            {
                int i = 0;
                for (; i < this->_count && this->_elements[i] != element; ++i);
                if (i == 8)
                    return false;
                for (i += 1; i < this->_count; ++i)
                    this->_elements[i - 1] = this->_elements[i];
                this->_elements[this->_count - 1] = 0;

                this->_count--;
                return true;
            }
            
            bool removed = false;
            for (int i = 0; i < 8; ++i)
                if (this->_childs[i]->Contains(*element) == AbstractCollider::Inside && this->_childs[i]->RemoveElement(element))
                {
                    removed = true;
                    break;
                }
            if (!removed)
                return false;

            this->_count--;
            if (this->_count == 8)
            {
                Octree<T>* childs[8];
                std::memcpy(childs, this->_childs, sizeof(this->_childs));

                int nb = 0;
                for (int i = 0; i < 8; ++i)
                {
                    for (int j = 0; j < childs[i]->_count; ++j)
                        this->_elements[nb++] = childs[i]->_elements[j];
                    Tools::Delete(childs[i]);
                }
            }
            return true;
        }

        template<class TFunc>
        void Foreach(TFunc function)
        {
            if (this->_count == 0)
                return;
            if (this->_count <= 8)
                for (int i = 0; i < this->_count; ++i)
                    function(*this->_elements[i]);
            else
                for (int i = 0; i < 8; ++i)
                    this->_childs[i]->Foreach<TFunc>(function);
        }

        template<class TFunc>
        void ForeachIn(AbstractCollider const& container, TFunc function)
        {
            if (this->_count == 0)
                return;
            if (this->_count <= 8)
            {
                for (int i = 0; i < this->_count; ++i)
                   if (container.Contains(*this->_elements[i]) != AbstractCollider::Outside)
                        function(*this->_elements[i]);
            }
            else
            {
                for (int i = 0; i < 8; ++i)
                    switch (container.Contains(*(this->_childs[i])))
                    {
                    case AbstractCollider::Inside:
                        this->_childs[i]->Foreach<TFunc>(function);
                        break;
                    case AbstractCollider::Intersecting:
                        this->_childs[i]->ForeachIn<TFunc>(container, function);
                        break;
                    case AbstractCollider::Outside:
                        break;
                    }
            }
        }

        template<class TFunc>
        void ForeachOut(AbstractCollider const& container, TFunc function)
        {
            if (this->_count == 0)
                return;
            if (this->_count <= 8)
            {
                for (int i = 0; i < this->_count; ++i)
                   if (container.Contains(*this->_elements[i]) == AbstractCollider::Outside)
                        function(*this->_elements[i]);
            }
            else
            {
                for (int i = 0; i < 8; ++i)
                    if (this->_childs[i] != 0)
                        switch (container.Contains(*(this->_childs[i])))
                        {
                        case AbstractCollider::Inside:
                            break;
                        case AbstractCollider::Intersecting:
                            this->_childs[i]->ForeachOut<TFunc>(container, function);
                            break;
                        case AbstractCollider::Outside:
                            this->_childs[i]->Foreach<TFunc>(function);
                            break;
                        }
            }
        }

        template<class TLogger>
        void Dump(TLogger& log) const
        {
            this->_Dump(1, log);
        }

        bool IsEmpty() const
        {
            return this->_count == 0;
        }

        unsigned int GetSize() const { return this->_size; }
        T* GetElement() const { return this->_element; }
        int GetCount() const { return this->_count; }

    private:
        template<class TLogger>
        void _Dump(int depth, TLogger& log) const
        {
            std::stringstream ss;
            ss.width(depth);
            ss << ">" << this->_count << " " << this->_size << " (" << this << ")\n";
            log << ss.str();
            if (this->_count > 8)
                for (int i = 0; i < 8; ++i)
                    if (this->_childs[i] != 0)
                        this->_childs[i]->_Dump(depth + 1, log);
        }
    };

}

#endif
