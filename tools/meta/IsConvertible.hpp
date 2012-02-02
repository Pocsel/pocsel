#ifndef __TOOLS_META_ISCONVERTIBLE_HPP__
#define __TOOLS_META_ISCONVERTIBLE_HPP__

namespace Tools { namespace Meta {

    template<typename From, typename To> struct IsConvertible
    {
    private:
        typedef char Yes;
        typedef struct {Yes BiggerThanYes[2];} No;

        static Yes f(To);
        static No f(...);

    public:
        static const bool result = sizeof(f(*((From*) 0))) == sizeof(Yes);
    };

}}

#endif
