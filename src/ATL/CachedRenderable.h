//
//  CachedRenderable.h
//  atl
//
//  Created by jacques tronconi on 21/03/2020.
//

#ifndef CachedRenderable_h
#define CachedRenderable_h

#include "Renderable.h"
#include "RenderCache.h"
#include "PerRendererCache.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Touchable.h"

namespace Atl
{
    //! @brief Interface for a Renderable that maintains a caching structure per renderer.
    //!
    //! Derives from this class if you want to have a PerRenderer Cache structure automatically
    //! managed for your Renderable. The cache structure is managed with a Touchable set of
    //! functions.
    //!
    //! The derived class must use \ref touch() to notify the PerRendererCache that it has 
    //! been modified. This action makes the cache to be rebuilt when a Renderer tries to
    //! render it.
    //!
    //! The derived class must implement \ref makeNewCache() function, which creates a new 
    //! RenderCache structure for the given Renderer. This function cannot return null.
    template < typename T > 
    class CachedRenderable : 
        virtual public Renderable, 
        virtual public Touchable
    {
    protected:
        //! @brief The caching structure.
        mutable PerRendererCache < T > mCaches;
        
        //! @brief Protects data over threads.
        mutable std::mutex mCachesMutex;
        
    public:
        //! @brief Destructor.
        virtual ~CachedRenderable() = default;

        //! @brief Creates a new RenderCache structure for the given Renderer.
        //! The returned cache is not built, as this comes after, and the cache is
        //! managed by this class. The returned value must not be null.
        virtual RenderCachePtr < T > makeNewCache(Renderer& rhs) = 0;
        
        //! @brief Asynchroneously renders this object into a RenderCommand.
        virtual std::future < void > render(RenderCommand& to) const {
            return std::async(std::launch::async, [this, &to]()
            {
                Renderer& renderer = to.renderer();
                
                mCachesMutex.lock();
                RenderCachePtr < T > cache = mCaches.cacheFor(renderer);
                
                if (!cache)
                {
                    mCachesMutex.unlock();
                    
                    // No cache found, just build it.
                    onCacheMiss(renderer);
                    const_cast < CachedRenderable& >(*this).build(renderer);
                    
                    return render(to).get();
                }
                
                if (mCaches.isCacheTouched(cache))
                {
                    mCaches.cleanCache(cache);
                    cache->build(renderer).get();
                }
                
                cache->render(to).get();
                mCachesMutex.unlock();
            });
        }
        
        //! @brief Creates a RenderCache for this object.
        virtual std::future < void > build(Renderer& renderer) {
            return std::async(std::launch::async, [this, &renderer]() 
            {
                RenderCachePtr < T > cache = makeNewCache(renderer);
                cache->build(renderer).get();
                
                std::lock_guard l(mCachesMutex);
                mCaches.addCache(cache);
            });
        }
        
        //! @brief Returns true if this renderable has a cache for given renderer.
        virtual bool hasCacheFor(Renderer& renderer) const {
            std::lock_guard l(mCachesMutex);
            return mCaches.cacheFor(renderer) != nullptr;
        }
        
        //! @brief Returns true if this CachedRenderable has been modified since last clean.
        //! This involves a call to \ref PerRendererCache::isAnyCacheTouched(). When true, a
        //! rebuilding of the cached data may be done with \ref build().
        virtual bool isTouched() const {
            std::lock_guard l(mCachesMutex);
            return mCaches.isAnyCacheTouched();
        }
        
        //! @brief Makes the renderable in a 'touched' state.
        virtual void touch() {
            std::lock_guard l(mCachesMutex);
            mCaches.touchAllCaches();
        }
        
        //! @brief Sets the renderable into a 'cleaned' state.
        virtual void clean() const {
            std::lock_guard l(mCachesMutex);
            mCaches.cleanAllCaches();
        }
        
        //! @brief Removes a Cache for a Renderer.
        virtual void removeCache(Renderer& renderer) {
            std::lock_guard l(mCachesMutex);
            
            RenderCachePtr < T > cache = mCaches.cacheFor(renderer);
            if (!cache) return;
            
            mCaches.removeCache(cache);
        }
        
        //! @brief Returns, in bytes, the memory used on the GPU RAM for this cache and for
        //! the given renderer.
        virtual std::size_t size(Renderer& renderer) const {
            std::lock_guard l(mCachesMutex);
            
            RenderCachePtr < T > cache = mCaches.cacheFor(renderer);
            if (!cache) return 0;
            
            return cache->size(renderer);
        }
        
    protected:
        
        //! @brief Called when a Cache is asked but not found.
        virtual void onCacheMiss(Renderer& renderer) const {}
    };
}

#endif /* CachedRenderable_h */
