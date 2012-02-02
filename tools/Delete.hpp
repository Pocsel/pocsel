#ifndef __TOOLS_DELETE_HPP__
#define __TOOLS_DELETE_HPP__

namespace Tools {

    template<typename T>
    static inline void Delete(T*& ptr)
    {
        delete ptr;
#ifdef DEBUG
        ptr = reinterpret_cast<T*>(1);
#endif
    }

    template<typename T>
    static inline void DeleteTab(T*& ptr)
    {
        delete [] ptr;
#ifdef DEBUG
        ptr = reinterpret_cast<T*>(1);
#endif
    }

}

#endif
