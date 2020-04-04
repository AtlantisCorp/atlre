//
//  Manager.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ATL_MANAGER_H
#define ATL_MANAGER_H

#include "Platform.h"
#include "Error.h"
#include "Singleton.h"
#include "Emitter.h"

#include <memory>
#include <vector>
#include <future>
#include <mutex>
#include <atomic>

namespace Atl
{
    class Resource;
    
    struct EXPORTED NoResourceFound : public Error
    { using Error::Error; };

    template < typename A, typename B, typename C >
    class Manager;

    //! @brief Defines a listener for a Generic listener.
    template < typename A, typename B, typename C >
    struct ManagerListener : virtual public ListenerBase 
    {
        //! @brief The manager type.
        using Manager = Manager < A, B, C >;

        //! @brief Destructor.
        virtual ~ManagerListener() = default;

        //! @brief Launched when a resource has been added.
        virtual void onResourceAdded(Manager&, B&){}

        //! @brief Launched when a resource has been removed.
        virtual void onResourceRemoved(Manager&, B&){}
    };
    
    //! @brief Defines a Generic Manager for a Resource.
    //! A Manager actually... manages the resource it has created. A Manager is made to
    //! be the most asynchroneous as possible, and as such, many functions returns a
    //! std::future and not directly the result.
    //! This can be used to load a Resource asynchroneously and wait for it only when we
    //! need it. Furthermore, resource finding is asynchroneous too because the Manager
    //! is optimized for a big amount of resource.
    //! @tparam ManagerClass The derived manager class for the Resource. This class must define
    //! the T::Resource field that is the actual resource we manage. \see The macro
    //! ATL_TYPEDEF_RESOURCE() for how it defines it.
    template <
    typename ManagerClass,
    typename ResourceClass,
    typename ListenerClass >
    class Manager : public Singleton < ManagerClass >,
                    public ListenerClass,
                    public Emitter
    {
    public:
        
        typedef ResourceClass Res;
        typedef std::shared_ptr < Res > ShRes;
        typedef std::future < ShRes > PromRes;

        //! @brief The listener for this class.
        typedef ListenerClass Listener;
        
    protected:
        
        mutable std::mutex mMutex;
        
        //! @brief The list of Resources in this Manager.
        std::vector < ShRes > mResources;
        
        //! @brief The currently used size with this Manager.
        std::atomic < std::size_t > mUsedSize;
        
        //! @brief The maximum size for this Manager.
        std::atomic < std::size_t > mMaxSize;
        
    public:
        
        //! @brief Changes the maximum size in bytes of this manager's resource pool.
        virtual void setMaxSize(std::size_t maxSize)
        {
            if (mUsedSize > maxSize)
                throw NotEnoughMemory("Manager", "setMaxSize", "maxSize %i is inferior to currently used size %i, please choose something bigger.", maxSize, mUsedSize.load());
            mMaxSize = maxSize;
        }
        
        //! @brief Tries to find a resource and, on failure, loads this resource.
        //! @param name The resource's name.
        //! @param args The resource's load args. Please refer to your resource's class
        //!     documentation for what parameters are accepted.
        //! @return A std::future with the resource, or throw an Error on failure.
        template < typename... Args >
        PromRes loadOrGet(const std::string& name, Args&&... args)
        {
            return std::async(std::launch::async, [this, &name, &args...]()
            {
                // Tries to find the resource by name.
                auto resource = find(name).get();
                return resource ? resource : load(name, std::forward < Args >(args)...).get();
            });
        }
        
        //! @brief Tries to find a resource by its name.
        //! @param name The resource's name.
        //! @return A std::future with the found resource. On failure, a null resource
        //! pointer is returned.
        PromRes find(const std::string& name)
        {
            return std::async(std::launch::async, [this, &name]()
            {
                std::lock_guard l(mMutex);
                
                for (ShRes resource : mResources)
                {
                    if (resource->name() == name)
                        return resource;
                }
                
                return ShRes{};
            });
        }
        
        //! @brief Loads a resource and adds it to this manager.
        //! @param name The resource's name. No verification is done upon the name's
        //! uniqueness and you are responsible for this. Please use \ref loadOrGet() instead
        //! if you want to check for a previously loaded resource.
        //! @param args The args to pass to the resource's loading function. Please refer
        //! to the resource's class documentation for a list of args to be passed.
        //! @return A std::future with the loaded resource. On failure, throw an Error.
        template < typename... Args >
        PromRes load(const std::string& name, Args&&... args)
        {
            return std::async(std::launch::async, [this, &name, &args...]()
            {
                // Tries to create a resource with this manager.
                ShRes resource = std::make_shared < Res >((ManagerClass&)*this, name);
                
                if (!resource)
                    throw NullError("Manager", "load", "Cannot allocate Resource %s.", name.data());
                
                // Adds this manager as a listener for 'ResourceLoadedEvent',
                // 'ResourceUnloadedEvent', 'ResourceCachedEvent' and 'ResourceUncachedEvent'.
                // Those events are listened to update the mCurrentTotalSize member and access
                // to the current size more easily.
                resource->addListener(this->shared_from_this());
                
                // Now let the loader loads itself (synchroneously to this function).
                resource->load(std::forward < Args >(args)...).get();
                
                // Adds the resource to this manager.
                {
                    std::lock_guard l(mMutex);
                    mResources.push_back(resource);
                }

                // Sends our event.
                send(&Listener::onResourceAdded, *this, *resource).get();
                
                // Next, return the resource.
                return resource;
            });
        }
        
        //! @brief Unloads a resource with specified name.
        //! @param name The resource's name to find. If the resource cannot be found,
        //! then a NoResourceFound Error is thrown by the launched thread.
        //! @param args The resource's args to be passed to the resource's unload
        //! function. Please refer to the resource's class documentation for what
        //! args can be passed to this function. Generally it is used for caching
        //! purpose.
        //! @return A std::future that holds the shared state of the unloading. If you
        //! want to wait for the future, please use \ref std::future::get().
        template < typename... Args >
        std::future < void > unload(const std::string& name, Args&&... args)
        {
            return std::async(std::launch::async, [this, &name, &args...]()
            {
                // Tries to find the resource.
                auto resource = find(name);
                
                if (!resource)
                    throw NoResourceFound("Manager", "unload", "Resource %s not found.",
                                          name.data());
                
                // Calls T::unload() with args.
                resource->unload(std::forward < Args >(args)...);
            });
        }
        
        //! @brief Removes the specified resource from this manager.
        //! @param name The resource's name to find. If the resource cannot be found,
        //! then a NoResourceFound Error is thrown by the launched thread.
        //! @return A std::future that holds the shared state of the removal. If you
        //! want to wait for the future, please use \ref std::future::get().
        std::future < void > remove(const std::string& name)
        {
            return std::async(std::launch::async, [this, &name]()
            {
                // Tries to find the resource.
                auto resource = find(name).get();
                
                if (!resource)
                    throw NoResourceFound("Manager", "unload", "Resource %s not found.",
                                          name.data());
                
                return remove(resource).get();
            });
        }
        
        //! @brief Removes a resource from this manager.
        //! @param resource A shared pointer to the resource to be removed.
        //! @return A std::future that holds the shared state of the removal. If you
        //! want to wait for the future, please use \ref std::future::get().
        std::future < void > remove(const ShRes& resource)
        {
            return std::async(std::launch::async, [this, &resource]()
            {
                bool didRemove = false;

                {
                    std::lock_guard l(mMutex);
                
                    // Tries to find the resource in the set.
                    auto iter = std::find(mResources.begin(), mResources.end(), resource);
                    
                    // If found, remove it.
                    if (iter != mResources.end())
                    {
                        mResources.erase(iter);
                        didRemove = true;
                    }
                }
                
                if (didRemove)
                    send(&Listener::onResourceRemoved, *this, (ResourceClass&)*resource).get();
            });
        }
        
        //! @brief Returns true if the size needed is available.
        bool isSizeAvailable(const std::size_t& sz) const
        {
            return availableSize() > sz;
        }
        
        //! @brief Returns the used size in bytes.
        std::size_t usedSize() const
        {
            return mUsedSize;
        }
        
        //! @brief Returns the available size in bytes.
        std::size_t availableSize() const
        {
            return maxSize() - usedSize();
        }
        
        //! @brief Returns the maximum size for this manager, in bytes.
        std::size_t maxSize() const
        {
            return mMaxSize;
        }
        
        //! @brief Answers to the resource's did load event.
        //! The resource's used size is added to this manager total size, in order
        //! to provide other resources an accurate number.
        void onResourceDidLoad(Resource& rhs)
        {
            Res& ref = reinterpret_cast < Res& >(rhs);
            mUsedSize.fetch_add(ref.usedSize());
        }
        
        //! @brief Answers to the resource's did unload event.
        void onResourceDidUnload(Resource& rhs)
        {
            Res& ref = reinterpret_cast < Res& >(rhs);
            mUsedSize.fetch_sub(ref.usedSize());
        }
    };
}

#endif /* ATL_MANAGER_H */
