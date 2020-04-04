//
//  Lockable.h
//  atlre
//
//  Created by jacques tronconi on 02/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ATL_LOCKABLE_H
#define ATL_LOCKABLE_H

#include <tuple>

namespace Atl
{
    //! @brief Interface for a lockable object.
    //! Instead of using \ref ATL_LOCKABLE_RESOURCE, just derive your class from
    //! this interface and implement lock/unlock. You will then use only LockableLockGuard
    //! (or LockableLockGuardCst) to lock your derived class. You can even use 
    //! LockableLocks to lock multiple lockables.
    struct EXPORTED Lockable
    {
        //! @brief Destructor.
        virtual ~Lockable() = default;
        //! @brief Locks the derived class.
        virtual void lock() const = 0;
        //! @brief Unlocks the derived class.
        virtual void unlock() const = 0;
    };

    //! @brief A lock guard that doesn't call try_lock. Please refer to std::lock_guard
    //! or std::scoped_lock source: we would have to implement try_lock() for all classes,
    //! which is unecessary.
    template < class... Mutexes >
    class LockableGuard;
    
    template < >
    class LockableGuard < >
    {
    public:
        explicit LockableGuard() {}
        ~LockableGuard() = default;
        
        LockableGuard(const LockableGuard&) = delete;
        LockableGuard& operator = (const LockableGuard&) = delete;
    };
    
    template < class Mutex >
    class LockableGuard < Mutex >
    {
        Mutex& mMutex;
    public:
        explicit LockableGuard(Mutex& mutex): mMutex(mutex) { mMutex.lock(); }
        ~LockableGuard() { mMutex.unlock(); }
        
        LockableGuard(const LockableGuard&) = delete;
        LockableGuard& operator = (const LockableGuard&) = delete;
    };
    
    namespace details
    {
        template < class L0 >
        void lock(L0& l0)
        {
            l0.lock();
        }
        
        template < class L0, class L1 >
        void lock(L0& l0, L1& l1)
        {
            l0.lock();
            l1.lock();
        }
        
        template < class L0, class L1, class L2, class... L3 >
        void lock(L0& l0, L1& l1, L2& l2, L3&... l3)
        {
            l0.lock();
            l1.lock();
            lock(l2, l3...);
        }
        
        template < class L0 >
        void unlock(L0& l0)
        {
            l0.unlock();
        }
        
        template < class L0, class L1 >
        void unlock(L0& l0, L1& l1)
        {
            l0.unlock();
            l1.unlock();
        }
        
        template < class L0, class L1, class L2, class... L3 >
        void unlock(L0& l0, L1& l1, L2& l2, L3&... l3)
        {
            l0.unlock();
            l1.unlock();
            unlock(l2, l3...);
        }
    }
    
    template < class... Mutexes >
    class LockableGuard
    {
        static_assert(sizeof...(Mutexes) > 1, "At least 2 lock types are needed.");
        typedef std::tuple < Mutexes&... > MutexTuple;
        
        MutexTuple mTuple;
        
    public:
        
        explicit LockableGuard(Mutexes&... args)
        : mTuple(args...)
        {
            typedef typename std::__make_tuple_indices<sizeof...(Mutexes)>::type MIndices;
            lock_unpack(MIndices{}, mTuple);
        }
        
        ~LockableGuard()
        {
            typedef typename std::__make_tuple_indices<sizeof...(Mutexes)>::type MIndices;
            unlock_unpack(MIndices{}, mTuple);
        }
        
    private:
        
        template < size_t... Indx >
        static void lock_unpack(std::__tuple_indices<Indx...>, MutexTuple& mt) {
            details::lock(std::get < Indx >(mt)...);
        }
        
        template < size_t... Indx >
        static void unlock_unpack(std::__tuple_indices<Indx...>, MutexTuple& mt) {
            details::unlock(std::get < Indx >(mt)...);
        }
    };
}

#endif // ATL_LOCKABLE_H
