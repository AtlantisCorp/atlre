//
//  RenderObjectManager.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef RenderObjectManager_h
#define RenderObjectManager_h

#include "Error.h"

#include <memory>
#include <vector>
#include <mutex>
#include <functional>

namespace Atl
{
    class Renderer;
    
    //! @brief A base manager for any RenderObject derived.
    //! This manager can be used only for derived classes from RenderObject, managed
    //! by the Renderer. Constructors can be added with \ref addClass(), a new object
    //! is created with \ref make() and then added to the manager with \ref add().
    //!
    //! @note
    //! The Renderer may have some specialized classes for derived render objects, if
    //! there is a need to.
    template < typename Class, typename Factory >
    class RenderObjectManager : public RenderObject
    {
    protected:
        
        typedef std::shared_ptr < Class > ClassPtr;
        typedef std::vector < ClassPtr > ClassList;
        mutable std::mutex mMutex;
        
        //! @brief The object's list.
        ClassList mObjects;
        
        //! @brief The factory for this manager.
        Factory mFactory;
        
    public:
        
        //! @brief Constructs an empty manager.
        RenderObjectManager(Renderer& rhs)
        : RenderObject(rhs)
        {
            
        }
        
        //! @brief Adds a class constructor.
        //! @param key The Key as defined in the used Factory template.
        //! @param fn The Constructor's function as defined in the used Factory
        //! template. Please refer to your Factory template for more about those
        //! args, as they are different for each render object.
        template < typename Callable >
        void addClass(const typename Factory::key_t& key, Callable fn)
        {
            std::lock_guard l(mMutex);
            mFactory.setConstructor(key, fn);
        }
        
        //! @brief Removes a class constructor.
        //! @param key The Key as defined in the used Factory template.
        void removeClass(const typename Factory::key_t& key)
        {
            std::lock_guard l(mMutex);
            mFactory.removeConstructor(key);
        }
        
        //! @brief Constructs a new render object.
        //! @param key The Key as defined in the used Factory template.
        //! @param args The arguments to pass to the Constructor.
        //! Please refer to your Factory template for more about those args,
        //! as they are different for each render object.
        template < typename... Args >
        ClassPtr make(const typename Factory::key_t& key,
                      Args&&... args)
        {
            return mFactory.construct(key, std::forward < Args >(args)...);
        }
        
        //! @brief Adds a new render object.
        virtual void add(const ClassPtr& rhs)
        {
            if (!rhs)
                throw NullError("RenderObjectManager", "add", "Null render object passed.");
            
            std::lock_guard l(mMutex);
            mObjects.push_back(rhs);
        }
        
        //! @brief Finds a render object as if using std::find_if.
        virtual ClassPtr find(std::function < bool(const ClassPtr&) > fn) const
        {
            std::lock_guard l(mMutex);
            
            for (const ClassPtr& ptr : mObjects)
            {
                if (fn(ptr))
                    return ptr;
            }
            
            return nullptr;
        }
        
        //! @brief Iterates over the render objects and call the given function.
        virtual void forEach(std::function < void(const ClassPtr&) > fn) const
        {
            std::lock_guard l(mMutex);
            
            for (const ClassPtr& ptr : mObjects)
            {
                fn(ptr);
            }
        }
        
        //! @brief Iterates over the render objects and call the given function.
        virtual void forEach(std::function < void(const ClassPtr&) > fn)
        {
            std::lock_guard l(mMutex);
            
            for (ClassPtr& ptr : mObjects)
            {
                fn(ptr);
            }
        }

        //! @brief Removes an object from this manager.
        virtual void remove(const ClassPtr& rhs)
        {
            std::lock_guard l(mMutex);
            mObjects.erase(std::find(mObjects.begin(), mObjects.end(), rhs));
        }
    };
}

#endif /* RenderObjectManager_h */
