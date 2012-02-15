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
        Octree<T>* _childs[8];
        unsigned int _size;
        T* _element;
        int _count;

    public:
        Octree(Vector3d const& position, unsigned int size)
            : AlignedCube(position, size),
            _size(size),
            _element(0),
            _count(0)
        {
            std::memset(this->_childs, 0, sizeof(this->_childs));
        }
        ~Octree()
        {
            for (int i = 0; i < 8; ++i)
                Tools::Delete(this->_childs[i]);
        }

        void InsertElement(T* element)
        {
            if (this->_count == 0)
                this->_element = element;
            else if (this->_count == 1)
            {
                unsigned int size = this->_size / 2;
                this->_childs[0] = new Octree<T>(this->GetPosition() + Vector3d(   0,    0,    0), size);
                this->_childs[1] = new Octree<T>(this->GetPosition() + Vector3d(   0,    0, size), size);
                this->_childs[2] = new Octree<T>(this->GetPosition() + Vector3d(   0, size, 0   ), size);
                this->_childs[3] = new Octree<T>(this->GetPosition() + Vector3d(   0, size, size), size);
                this->_childs[4] = new Octree<T>(this->GetPosition() + Vector3d(size,    0, 0   ), size);
                this->_childs[5] = new Octree<T>(this->GetPosition() + Vector3d(size,    0, size), size);
                this->_childs[6] = new Octree<T>(this->GetPosition() + Vector3d(size, size, 0   ), size);
                this->_childs[7] = new Octree<T>(this->GetPosition() + Vector3d(size, size, size), size);

                bool ok = false;
                for (int i = 0; i < 8; ++i)
                {
                    if (this->_childs[i]->Contains(*this->_element) == AbstractCollider::Inside)
                    {
                        if (ok)
                            Tools::log << "AAAAAAAAAHHHHHHHH\n";
                        this->_childs[i]->InsertElement(this->_element);
                        ok = this->_childs[i]->_element == this->_element;
                    }
                    if (this->_childs[i]->Contains(*element) == AbstractCollider::Inside)
                        this->_childs[i]->InsertElement(element);
                }
                this->_element = 0;
            }
            else
            {
                for (int i = 0; i < 8; ++i)
                    if (this->_childs[i]->Contains(*element) == AbstractCollider::Inside)
                        this->_childs[i]->InsertElement(element);
            }
            this->_count++;
        }

        void ReplaceElement(T* element)
        {
            if (this->_count == 0)
                throw std::logic_error("Replace an inexistant item");
            else if (this->_count == 1)
                this->_element = element;
            else
            {
                for (int i = 0; i < 8; ++i)
                    if (this->_childs[i]->Contains(*element) == AbstractCollider::Inside)
                        this->_childs[i]->ReplaceElement(element);
            }
        }

        bool RemoveElement(T* element)
        {
            if (this->_count == 0)
                return false;
            if (this->_count == 1)
            {
                if (this->_element != element)
                    return false;
                this->_element = 0;
            }
            else
            {
                bool removed = false;
                for (int i = 0; i < 8; ++i)
                    if (this->_childs[i]->Contains(*element) == AbstractCollider::Inside)
                        removed = removed || this->_childs[i]->RemoveElement(element);
                if (!removed)
                    return false;
            }
            this->_count--;
            if (this->_count == 1)
            {
                for (int i = 0; i < 8; ++i)
                {
                    if (this->_childs[i]->_count == 1)
                        this->_element = this->_childs[i]->_element;
                    delete this->_childs[i];
                    this->_childs[i] = 0;
                }
            }
            return true;
        }

        template<class TFunc>
        void Foreach(TFunc function)
        {
            if (this->_count == 0)
                return;
            if (this->_element)
                function(*this->_element);
            else
                for (int i = 0; i < 8; ++i)
                    this->_childs[i]->Foreach<TFunc>(function);
        }

        template<class TFunc>
        void ForeachIn(AbstractCollider const& container, TFunc function)
        {
            if (this->_count == 0)
                return;
            if (this->_element)
            {
                if (container.Contains(*this->_element) != AbstractCollider::Outside)
                    function(*this->_element);
            }
            else
            {
                for (int i = 0; i < 8; ++i)
                    if (this->_childs[i] != 0)
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
            if (this->_element)
            {
                if (container.Contains(*this->_element) == AbstractCollider::Outside)
                    function(*this->_element);
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

        bool IsEmpty() const
        {
            return this->_count == 0;
        }
    };

}

#endif
