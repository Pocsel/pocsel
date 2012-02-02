#ifndef __TOOLS_MEMORYPOOL_HPP__
#define __TOOLS_MEMORYPOOL_HPP__

#include <list>

namespace Tools
{

    template<class T>
    class MemoryPool
    {
    private:
        enum State
        {
            Free = 0,
            InUse,
            InUseRaw
        };

        std::list<T*> _pool;
        std::list<State*> _inUse;
        T* _freeCache[8];
        State* _freeStateCache[8];
        size_t _sizeTotal;
        size_t _sizeUsed;
        size_t _blockSize;

        void _IncreaseSize();
        T* _GetFreeBlock(State type = InUse);

    public:
        MemoryPool(int blockSize = 2048);
        ~MemoryPool();

        T& CreateElement();
        template<class P1> T& CreateElement(P1 p1);
        template<class P1, class P2> T& CreateElement(P1 p1, P2 p2);
        template<class P1, class P2, class P3> T& CreateElement(P1 p1, P2 p2, P3 p3);
        template<class P1, class P2, class P3, class P4> T& CreateElement(P1 p1, P2 p2, P3 p3, P4 p4);
        void DestroyElement(T& element);

        T* AllocateElement();
        void FreeElement(T* ptr);
    };

    template<class T>
    MemoryPool<T>::MemoryPool(int blockSize) :
        _sizeTotal(blockSize),
        _sizeUsed(0),
        _blockSize(blockSize)
    {
        this->_pool.push_back((T*)std::malloc(this->_sizeTotal * sizeof(T)));
        State* tmp = (State*)std::malloc(this->_sizeTotal * sizeof(State));
        this->_inUse.push_back(tmp);
        std::memset(tmp, 0, this->_blockSize * sizeof(State));
        std::memset(this->_freeCache, 0, sizeof(this->_freeCache));
    }

    template<class T>
    MemoryPool<T>::~MemoryPool()
    {
        auto it2 = this->_inUse.begin();
        for (auto it = this->_pool.begin(), ite = this->_pool.end(); it != ite; ++it, ++it2)
        {
            for (size_t pos = 0; pos < this->_blockSize; ++pos)
            {
                if ((*it2)[pos] == InUse)
                    (*it)[pos].~T();
            }
            std::free(*it);
            std::free(*it2);
        }
    }

    template<class T>
    void MemoryPool<T>::_IncreaseSize()
    {
        this->_sizeTotal += this->_blockSize;
        this->_pool.push_back((T*)std::malloc(this->_sizeTotal * sizeof(T)));
        State* tmp = (State*)std::malloc(this->_sizeTotal * sizeof(State));
        this->_inUse.push_back(tmp);
        std::memset(tmp, 0, this->_blockSize * sizeof(State));
    }

    template<class T>
    T* MemoryPool<T>::_GetFreeBlock(State type)
    {
        auto it = this->_pool.begin();
        auto itUse = this->_inUse.begin();
        if (this->_sizeTotal == this->_sizeUsed)
        {
            this->_IncreaseSize();
            it = --(this->_pool.end());
            itUse = --(this->_inUse.end());
        }
        else if (this->_freeCache[0])
        {
            T* ptr = this->_freeCache[0];
            *(this->_freeStateCache[0]) = type;
            for (size_t i = 1; i < sizeof(this->_freeCache) / sizeof(*(this->_freeCache)); ++i)
            {
                this->_freeCache[i - 1] = this->_freeCache[i];
                this->_freeStateCache[i - 1] = this->_freeStateCache[i];
            }
            this->_freeCache[(sizeof(this->_freeCache) / sizeof(*(this->_freeCache))) - 1] = 0;
            ++this->_sizeUsed;
            return ptr;
        }
        for (auto ite = this->_pool.end(); it != ite; ++it, ++itUse)
            for (size_t pos = 0; pos < this->_blockSize; ++pos)
                if ((*itUse)[pos] == Free)
                {
                    (*itUse)[pos] = type;
                    ++this->_sizeUsed;
                    return (*it + pos);
                }
        return 0;
    }

    template<class T>
    T& MemoryPool<T>::CreateElement()
    {
        T* ptr = this->_GetFreeBlock();
        new (ptr) T();
        return *ptr;
    }

    template<class T>
    template<class P1> T& MemoryPool<T>::CreateElement(P1 p1)
    {
        T* ptr = this->_GetFreeBlock();
        new (ptr) T(p1);
        return *ptr;
    }

    template<class T>
    template<class P1, class P2> T& MemoryPool<T>::CreateElement(P1 p1, P2 p2)
    {
        T* ptr = this->_GetFreeBlock();
        new (ptr) T(p1, p2);
        return *ptr;
    }

    template<class T>
    template<class P1, class P2, class P3> T& MemoryPool<T>::CreateElement(P1 p1, P2 p2, P3 p3)
    {
        T* ptr = this->_GetFreeBlock();
        new (ptr) T(p1, p2, p3);
        return *ptr;
    }

    template<class T>
    template<class P1, class P2, class P3, class P4> T& MemoryPool<T>::CreateElement(P1 p1, P2 p2, P3 p3, P4 p4)
    {
        T* ptr = this->_GetFreeBlock();
        new (ptr) T(p1, p2, p3, p4);
        return *ptr;
    }

    template<class T>
    void MemoryPool<T>::DestroyElement(T& element)
    {
        this->Free(&element);
        element.~T();
    }

    template<class T>
    T* MemoryPool<T>::AllocateElement()
    {
        return (this->_GetFreeBlock(InUseRaw));
    }

    template<class T>
    void MemoryPool<T>::FreeElement(T* ptr)
    {
        assert(false && "TODO FAIT CHIER");
    }

}

#endif
