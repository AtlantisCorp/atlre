
//
//  Resource.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef Resource_h
#define Resource_h

#include "Platform.h"
#include "Error.h"
#include "ResourceListener.h"
#include "Manager.h"
#include "LoaderDb.h"
#include "File.h"
#include "Emitter.h"
#include "Lockable.h"

#include <string>
#include <mutex>
#include <atomic>
#include <memory>

namespace Atl
{
    struct EXPORTED NoLoaderFound: public Error
    { using Error::Error; };
    
    struct EXPORTED NoSizeAvailable: public Error
    { using Error::Error; };
    
    struct EXPORTED AlreadyLoaded: public Error
    { using Error::Error; };

    struct EXPORTED NameAlreadyExists: public Error
    { using Error::Error; };
    
    //! @brief Base class for a resource.
    //! A Resource is an object that can be loaded and unloaded. From this simple definition,
    //! a resource englobates all file - loaded objects (archives, models, ...) and other objects
    //! that can be loaded with a Loader.
    //!
    //! @warning
    //! A Resource cannot be an object created from, for example, a Renderer. Why ? Because a
    //! renderer - created object cannot be loaded / unloaded. However, a Resource can hold an
    //! object created from a Renderer (a Model holds a Mesh object for each Renderer).
    //!
    //! @section Deriving from Resource
    //! Ideally you should always derive from \ref TResource. A Resource should define its
    //! Manager, its Loader and its LoaderDb.
    class EXPORTED Resource : virtual public Emitter
    {
        //! @brief The unique name for this resource.
        std::string mName;
        
        //! @brief Mutex for the data.
        mutable std::mutex mMutex;
        
    public:
        typedef ResourceListener Listener;
        
    protected:
        
        //! @brief Available states for a resource.
        enum States
        {
            Cached = 0x01,
            Loaded = 0x02
        };
        
        //! @brief The current resource's state.
        std::atomic < unsigned > mState;
        
    public:
        
        //! @brief Constructs a new Resource.
        Resource(const std::string& name);
        
        //! @brief Default destructor.
        virtual ~Resource() = default;
        
        // This function should be present in your derived class.
        // std::future < void > load(...);
        
        // This function should be present in your derived class.
        // std::future < void > unload(...);
        
        //! @brief Returns the state for this resource.
        unsigned state() const;
        
        //! @brief Returns the name of this resource.
        std::string name() const;
        
        //! @brief Returns true if state is loaded.
        bool isLoaded() const;
        
        //! @brief Returns true if state is cached.
        bool isCached() const;
        
        //! @brief Should return the used size, in bytes, by the resource.
        virtual std::size_t usedSize() const = 0;
        
    protected:
        
        //! @brief Changes the current state.
        void setState(unsigned state);
    };
    
    //! @brief A Helper class for a derived resource type.
    template < typename T, typename ManagerClass, typename LoaderT = LoaderTrait < T > >
    class TResource : public Resource
    {
    public:
        
        typedef ManagerClass Manager;
        typedef LoaderDb < LoaderT > LoaderDb;
        typedef typename LoaderDb::shared_loader_type ShLoader;
        typedef typename LoaderDb::loader_type Loader;
        
    public:
        
        //! @brief The manager that created this resource.
        Manager& mManager;
        
    public:
        
        TResource(Manager& manager, const std::string& name)
        : Resource(name), mManager(manager)
        {
            
        }
        
        //! @brief Returns the manager of this resource.
        Manager& manager()
        {
            return mManager;
        }
        
        //! @brief Returns the global instance of LoaderDb of this resource.
        LoaderDb& loaderDb() const
        {
            return LoaderDb::Get();
        }
        
        //! @brief Asserts all requirements are fullfilled to load this resource.
        //! @param loader The loader with which to load this resource.
        //! @param filename The filename to load.
        virtual std::future < void > preload(const ShLoader& loader, const std::string& filename)
        {
            return std::async(std::launch::async, [this, &loader, &filename](){
                if (!loader || filename.empty())
                    throw NullError("TResource", "preload", "Null preload args.");
                
                if (isLoaded())
                    throw AlreadyLoaded("TResource", "preload", "Resource %s already loaded.", name().data());
                
                std::size_t neededSize = loader->neededSize(filename, Params{});
                
                if (!manager().isSizeAvailable(neededSize))
                    throw NoSizeAvailable("TResource", "preload", "Size %i unavailable to load file %s.", neededSize, filename.data());
                
                send(&T::Listener::onResourceWillLoad, (Resource&)*this);
            });
        }
        
        //! @brief Loads the resource with only a Params object.
        virtual std::future < void > load(const std::string& filename, const Params& params)
        {
            return std::async(std::launch::async, [this, &filename, &params](){
                auto loader = findLoaderForFile(filename);
                preload(loader, filename).get();
                loader->load(dynamic_cast < T& >(*this), filename, params);
                postload(filename).get();
            });
        }
        
        virtual std::future < void > postload(const std::string& filename)
        {
            return std::async(std::launch::async, [this](){
                setState(Loaded);
                send(&T::Listener::onResourceDidLoad, (Resource&)*this);
            });
        }
        
    protected:
        
        //! @brief Finds a loader for this resource, based on an analyse of the extension
        //! of the filename. A Null pointer is returned if no loader is found.
        ShLoader findLoaderForFile(const std::string& filename)
        {
            const std::string extension = File::Extension(filename);
            return LoaderDb::Get().find(extension);
        }
    };

    //! @brief Makes only the Pointer and List types for a Resource.
#   define ATL_BASE_RESOURCE(T) \
    typedef std::shared_ptr < T > T##Ptr; \
    typedef std::vector < T##Ptr > T##List;

    //! @brief Makes Loader and LoaderDb Typedefs.
#   define ATL_LOADER_RESOURCE(T) \
    typedef LoaderTrait < T > T##Loader; \
    typedef LoaderDb < T##Loader > T##LoaderDb; 

    //! @brief Creates some LockGuard for T and const T. Your class must
    //! derive from Lockable.
#   define ATL_LOCKABLE_RESOURCE(T) \
    typedef LockableGuard < T > T##LockGuard; \
    typedef LockableGuard < const T > T##LockGuardCst;
}

#endif /* Resource_h */
