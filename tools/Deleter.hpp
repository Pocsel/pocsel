#ifndef __TOOLS_DELETER_HPP__
#define __TOOLS_DELETER_HPP__

namespace Tools {

    template<typename T>
    struct Deleter
    {
        private:
        struct _Deleter
        {
            bool del;
            _Deleter() : del(true)
            {
            }
            void operator () (T* ptr)
            {
                if (del == true)
                    Tools::Delete(ptr);
            }
        };

        public:
        static std::shared_ptr<T> CreatePtr(T* ptr)
        {
            return std::shared_ptr<T>(ptr, _Deleter());
        }

        static void CancelDelete(std::shared_ptr<T>& shared)
        {
            std::get_deleter<_Deleter, T>(shared)->del = false;
        }

        static T* StealPtr(std::shared_ptr<T>& shared)
        {
            CancelDelete(shared);
            return shared.get();
        }
    };

}

#endif

