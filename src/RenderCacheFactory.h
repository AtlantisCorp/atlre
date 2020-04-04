//
//  RenderCacheFactory.h
//  atl
//
//  Created by jacques tronconi on 22/03/2020.
//

#ifndef RenderCacheFactory_h
#define RenderCacheFactory_h

#include "Platform.h"
#include "RenderCache.h"

#include <map>
#include <mutex>
#include <typeindex>
#include <any>
#include <memory>

namespace Atl
{
    //! @brief A Helper factory to create different types of RenderCaches.
    //! Each RenderCache is specific to its derived class. For example, we may have a GL cache
    //! for SubModel, but also for Model, for Material, etc. So, our factory should be able to
    //! create all those kind of caches, regardless of the final type.
    //!
    //! In order to achieve type erasure, the factory has a 'base type' (i.e. SubModel) which defines
    //! the RenderCache base type (i.e. RenderCache < SubModel >). The 'base type' is the stored map
    //! key. The values are the 'derived type', which is for example GLSubModelRenderCache. The
    //! Constructor is a lambda that takes in arguments the RenderCache args and returns a
    //! RenderCachePtr for the base type.
    //!
    //! This type erasure method requires you not to register invalid function pointers. To add some
    //! type security, we create the lambda directly in this class, in a private member. This ensure
    //! the function created is conform to the RenderCache constructor.
    //!
    //! @note
    //! The use of std::any let us have another type-security feature. The function provided must be
    //! of the exact type of a RenderCache constructor.
    //!
    //! @note
    //! We cannot use the Factory generic template because we need to instantiate one RenderCache < T >,
    //! and thus we cannot have a simple return type. Other types can use the generic Factory
    //! template to generate a factory, like RenderCommand.
    class EXPORTED RenderCacheFactory
    {
        mutable std::mutex mMutex;
        
        //! @brief The map with the RenderCache base type as key, and the function as a std::any.
        std::map < std::type_index, std::any > mCacheConstructors;
        
    public:
        
        //! @brief Constructs a new RenderCache for the given object.
        //! The RenderCache is not yet ready to be used. \ref RenderCache::build() must be used
        //! before rendering the cache.
        template < typename T >
        RenderCachePtr < T > construct(Renderer& renderer, T& object) {
            std::lock_guard l(mMutex);
            
            auto it = mCacheConstructors.find(typeid(T));
            
            if (it == mCacheConstructors.end())
                throw FactoryNullConstructor("RenderCacheFactory", "construct", "Cannot find Constructor for RenderCache < %s >.", typeid(T).name());
            
            typedef std::function < RenderCachePtr < T >(Renderer&, T&) > Constructor;
            auto fn = std::any_cast < Constructor >(it->second);
            
            return fn(renderer, object);
        }
        
    private:
        
        //! @brief Sets a constructor for the given RenderCache base type.
        //! This function ensure the passed function is of the right type. However, it is used
        //! only from \ref makeConstructor(), which create for you the correct lambda.
        template < typename T >
        void setConstructor(std::function < RenderCachePtr < T >(Renderer&, T&) > fn) {
            std::lock_guard l(mMutex);
            mCacheConstructors[typeid(T)] = fn;
        }
        
    public:
        
        //! @brief Creates a lambda for the RenderCache and calls \ref setConstructor.
        //! The lambda created is of the form RenderCachePtr < T >(Renderer&, T&). This lambda
        //! only calls std::make_shared < Derived >(renderer, t) which makes it able to create
        //! a RenderCachePtr < T > object.
        template < typename T, typename Derived >
        void makeConstructor() {
            typedef std::function < RenderCachePtr < T >(Renderer&, T&) > Constructor;
            Constructor fn = [](Renderer& ren, T& rhs) { return std::make_shared < Derived >(ren, rhs); };
            setConstructor < T >(fn);
        }
        
        //! @brief Clears the constructor for given RenderCache base type.
        template < typename T >
        void clearConstructor() {
            std::lock_guard l(mMutex);
            auto it = mCacheConstructors.find(typeid(T));
            mCacheConstructors.erase(it);
        }
        
        //! @brief Clears all constructors.
        //! Uses this function only if you then reload the Renderer.
        void clearAllConstructor() {
            std::lock_guard l(mMutex);
            mCacheConstructors.clear();
        }
    };
}

#endif /* RenderCacheFactory_h */
