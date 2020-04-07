//
//  ShareableManager.h
//  atl
//
//  Created by jacques tronconi on 07/04/2020.
//

#ifndef ATL_SHAREABLEMANAGER_H
#define ATL_SHAREABLEMANAGER_H

#include "Platform.h"

namespace Atl
{
    //! @brief Very simple manager for classes that uses ATL_SHAREABLE() macro.
    //! This manager defines very simple add/find/remove functions and a makeNew() templated
    //! function that calls, for a derived class of your managed class, the New static 
    //! member declared with ATL_SHAREABLE() macro.
    template < class T >
    class ShareableManager 
    {
        //! @brief List of Shareable derived objects.
        std::vector < std::shared_ptr < T > > mShareables;

        //! @brief The mutex.
        mutable std::mutex mMutex;

    public:

        //! @brief Calls D::New() with some args.
        //! This constructs a new D using the constructor with given args, if available. Any error
        //! in the argument list will resolve into a compilation error. 
        //! D must declare ATL_SHAREABLE() in its public body, and be statically castable to this
        //! manager's base class (T).
        template < typename D, typename... Args >
        std::shared_ptr < T > makeNew(Args&&... args) const {
            return std::static_pointer_cast < T >(D::New(std::forward < Args >(args)...));
        }

        //! @brief Adds a new shareable instance.
        void add(const std::shared_ptr < T >& shareable) {
            std::lock_guard l(mMutex);
            mShareables.push_back(shareable);
        }

        //! @brief Finds a new shareable instance with the given function.
        //! The callable function is of the form ``bool(const std::shared_ptr < T >&)`` and 
        //! returns true if the shareable is the one looked for.
        //! \return nullptr if not found.
        template < typename Callable >
        std::shared_ptr < T > find(Callable&& callable) const {
            std::lock_guard l(mMutex);
            auto it = std::find_if(mShareables.begin(), mShareables.end(), callable);
            return it == mShareables.end() ? nullptr : *it;
        }

        //! @brief Removes the given shareable if found in the list.
        void remove(const std::shared_ptr < T >& shareable) {
            std::lock_guard l(mMutex);
            auto it = std::find(mShareables.begin(), mShareables.end(), shareable);
            if (it != mShareables.end()) mShareables.erase(it);
        }
    };
}

#endif // ATL_SHAREABLEMANAGER_H