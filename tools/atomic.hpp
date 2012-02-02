#ifndef __TOOLS_ATOMIC_HPP__
#define __TOOLS_ATOMIC_HPP__

#ifndef _WIN32

# define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

/* Test for GCC > 4.5.0 */
# if GCC_VERSION >= 40500
#  include <atomic>
# else
#  include <cstdatomic>
# endif
#else

# undef GCC_VERSION

#include <boost/memory_order.hpp>
#include <boost/detail/interlocked.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4146)
#endif

namespace std
{

    static inline void full_fence(void)
    {
        long tmp;
        BOOST_INTERLOCKED_EXCHANGE(&tmp, 0);
    }

    static inline void fence_after_load(boost::memory_order order)
    {
        switch(order)
        {
        case boost::memory_order_seq_cst:
            full_fence();
        case boost::memory_order_acquire:
        case boost::memory_order_acq_rel:
        default:;
        }
    }

    /**
     * Implementation très basique de std::atomic<T>
     * Repris depuis: http://www.chaoticmind.net/~hcb/projects/boost.atomic/
     */
    template<class T>
    class atomic
    {
    public:
        typedef T integral_type;

    private:
        T _i;

        atomic(atomic<T> const&);
        atomic<T>& operator =(atomic<T> const&);

    public:
        atomic() {}
        explicit atomic(T v) : _i(v) {}

        T load(boost::memory_order order=boost::memory_order_seq_cst) const volatile
        {
            T v = *reinterpret_cast<volatile const T *>(&_i);
            fence_after_load(order);
            return v;
        }

        void store(T v, boost::memory_order order=boost::memory_order_seq_cst) volatile
        {
            if (order!=boost::memory_order_seq_cst)
                *reinterpret_cast<volatile T *>(&_i) = v;
            else
                exchange(v);
        }
        bool compare_exchange_strong(T &expected, T desired, boost::memory_order success_order, boost::memory_order failure_order) volatile
        {
            T prev = expected;
            expected = (T)BOOST_INTERLOCKED_COMPARE_EXCHANGE((long *)(&_i), (long)desired, (long)expected);
            bool success = (prev == expected);
            return success;
        }
        bool compare_exchange_weak(T &expected, T desired, boost::memory_order success_order, boost::memory_order failure_order) volatile
        {
            return compare_exchange_strong(expected, desired, success_order, failure_order);
        }
        T exchange(T r, boost::memory_order order=boost::memory_order_seq_cst) volatile
        {
            return (T)BOOST_INTERLOCKED_EXCHANGE((long *)&_i, (long)r);
        }
        T fetch_add(T c, boost::memory_order order=boost::memory_order_seq_cst) volatile
        {
            return (T)BOOST_INTERLOCKED_EXCHANGE_ADD((long *)&_i, c);
        }

        integral_type fetch_sub(integral_type c, boost::memory_order order=boost::memory_order_seq_cst) volatile
        {
            return fetch_add(-c, order);
        }
           integral_type fetch_and(integral_type c, boost::memory_order order=boost::memory_order_seq_cst) volatile
        {
            integral_type o = load(boost::memory_order_relaxed), n;
            do { n = o & c; } while (!compare_exchange_weak(o, n, order, boost::memory_order_relaxed));
            return o;
        }
        integral_type fetch_or(integral_type c, boost::memory_order order=boost::memory_order_seq_cst) volatile
        {
            integral_type o = load(boost::memory_order_relaxed), n;
            do { n = o | c; } while (!compare_exchange_weak(o, n, order, boost::memory_order_relaxed));
            return o;
        }
        integral_type fetch_xor(integral_type c, boost::memory_order order=boost::memory_order_seq_cst) volatile
        {
            integral_type o = load(boost::memory_order_relaxed), n;
            do { n = o ^ c; } while (!compare_exchange_weak(o, n, order, boost::memory_order_relaxed));
            return o;
        }

        operator integral_type(void) const volatile { return load(); }
        integral_type operator=(integral_type v) volatile { store(v); return v; }

        integral_type operator&=(integral_type c) volatile { return fetch_and(c) & c; }
        integral_type operator|=(integral_type c) volatile { return fetch_or(c) | c; }
        integral_type operator^=(integral_type c) volatile { return fetch_xor(c) ^ c; }

        integral_type operator+=(integral_type c) volatile { return fetch_add(c) + c; }
        integral_type operator-=(integral_type c) volatile { return fetch_sub(c) - c; }

        integral_type operator++(void) volatile { return fetch_add(1) + 1; }
        integral_type operator++(int) volatile { return fetch_add(1); }
        integral_type operator--(void) volatile { return fetch_sub(1) - 1; }
        integral_type operator--(int) volatile { return fetch_sub(1); }

        bool is_lock_free(void) const volatile { return true; }
    };

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

#endif
