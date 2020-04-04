//
//  LockableManager.h
//  atlre
//
//  Created by jacques tronconi on 02/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ATL_LOCKABLEMANAGER_H
#define ATL_LOCKABLEMANAGER_H

#include "Lockable.h"
#include "Emitter.h"

namespace Atl
{
    template < typename T >
    class LockableManager;

    template < typename T >
    using LockableManagerPtr = std::shared_ptr < LockableManager < T > >;
    template < typename T >
    using LockableManagerList = std::vector < LockableManagerPtr < T > >;

    //! @brief The listener to LockableManager.
    template < typename T >
    struct LockableManagerListener : 
        virtual public ListenerBase
    {
        //! @brief The manager.
        typedef LockableManager < T > Manager;

        //! @brief Destructor.
        virtual ~LockableManagerListener() = default;
        //! @brief Launched when a T has been added.
        virtual void onObjectAdded(Manager&, T&){}
        //! @brief Launched when a T has been removed.
        virtual void onObjectRemoved(Manager&, T&){}
    };

    template < typename T >
    class LockableManager : 
        virtual public Emitter,
        virtual public Lockable
    {
        //! @brief The mutex.
        mutable std::mutex mMutex;

        //! @brief The pointer to T.
        typedef std::shared_ptr < T > Pointer;
        //! @brief The vector of Pointer.
        typedef std::vector < Pointer > List;

        //! @brief The list of objects.
        List mPointers;

    public: 
        ATL_SHAREABLE(LockableManager)

        //! @brief The listener for this manager.
        typedef LockableManagerListener < T > Listener;

        //! @brief Destructor.
        virtual ~LockableManager() = default;

        //! @brief Adds a non null pointer to this manager.
        //! @return Returns the index of the added object.
        virtual std::size_t add(const Pointer& rhs) {
            if (!rhs)
                throw NullError("LockableManager", "add", "Null Pointer.");

            std::size_t index = 0;

            {
                std::lock_guard l(mMutex);
                index = mPointers.size();
                mPointers.push_back(rhs);
            }

            send(&Listener::onObjectAdded, *this, *rhs);
            return index;
        }

        //! @brief Removes the pointer from this manager.
        virtual void remove(const Pointer& rhs) {
            if (!rhs)
                throw NullError("LockableManager", "remove", "Null Pointer.");

            {
                std::lock_guard l(mMutex);
                mPointers.erase(std::find(mPointers.begin(), mPointers.end(), rhs));
            }

            send(&Listener::onObjectRemoved, *this, *rhs);
        }

        //! @brief Returns the number of Objects in the manager.
        //! This function should be called after LockableManagerGuard or LockableManagerGuardCst
        //! because it is not thread-safe. You can then call \ref objectAt() to get objects.
        virtual std::size_t objectsCount() const {
            return mPointers.size();
        }

        //! @brief Returns the object at given index.
        virtual T& objectAt(std::size_t index) {
            return *mPointers[index];
        }

        //! @brief Returns the object at given index.
        virtual const T& objectAt(std::size_t index) const {
            return *mPointers[index];
        }

        //! @brief Returns a reference to the list of objects.
        //! Don't forget to lock the manager to ensure concurrency.
        virtual List& objects() { 
            return mPointers;
        }

        //! @brief Returns a reference to the list of objects.
        //! Don't forget to lock the manager to ensure concurrency.
        virtual const List& objects() const { 
            return mPointers;
        }

        //! @brief Locks the manager.
        virtual void lock() const {
            mMutex.lock();
        }

        //! @brief Unlocks the manager.
        virtual void unlock() const {
            mMutex.unlock();
        }
    };
}

#endif // ATL_LOCKABLEMANAGER_H
