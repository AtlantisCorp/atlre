//
//  PerRendererCache.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef PerRendererCache_h
#define PerRendererCache_h

#include "RenderCache.h"
#include "Touchable.h"
#include "Error.h"

#include <vector>

namespace Atl
{
    //! @brief A structure deriving from Touchable that holds a RenderCache.
    template < typename T >
    struct PerRendererCacheInfos : public TimeTouchable
    {
        //! @brief The render cache.
        RenderCachePtr < T > renderCache;
        
        //! @brief Constructs a new PerRendererCacheInfos.
        PerRendererCacheInfos(const RenderCachePtr < T >& rhs): renderCache(rhs) {}
        PerRendererCacheInfos(const PerRendererCacheInfos& rhs): renderCache(rhs.renderCache) {}
    };
    
    //! @brief An Error when a PerRendererCacheInfos is not found.
    struct PerRendererCacheNoInfosFound : public Error
    { using Error::Error; };
    
    //! @brief A per-renderer cache manager.
    template < typename T >
    class PerRendererCache
    {
        //! @brief The caches registered.
        std::vector < PerRendererCacheInfos < T > > mRenderCaches;
        
        //! @brief The mutex.
        mutable std::mutex mMutex;
        
        PerRendererCache(const PerRendererCache&) = delete;
        PerRendererCache& operator = (const PerRendererCache&) = delete;
        
    public:
        //! @brief Constructs an empty PerRendererCache.
        PerRendererCache() = default;
        
        //! @brief Default destructor.
        virtual ~PerRendererCache() = default;
        
        //! @brief Returns the cache for a renderer, or null if no cache is found.
        RenderCachePtr < T > cacheFor(Renderer& renderer) const {
            std::lock_guard l(mMutex);
            
            for (const PerRendererCacheInfos < T >& infos : mRenderCaches)
            {
                if (infos.renderCache && infos.renderCache->isFrom(renderer))
                    return infos.renderCache;
            }
            
            return nullptr;
        }
        
        //! @brief Adds a cache in this manager.
        void addCache(const RenderCachePtr < T >& cache) {
            std::lock_guard l(mMutex);
            mRenderCaches.push_back(PerRendererCacheInfos(cache));
        }
        
        //! @brief Removes a cache from this manager.
        void removeCache(const RenderCachePtr < T >& cache) {
            std::lock_guard l(mMutex);
            
            auto it = std::find_if(mRenderCaches.begin(), mRenderCaches.end(),
                                   [cache](const PerRendererCacheInfos < T >& rhs){
                return rhs.renderCache == cache;
            });
            
            if (it != mRenderCaches.end())
                mRenderCaches.erase(it);
        }
        
        //! @brief Removes all caches from this manager.
        void clear() {
            std::lock_guard l(mMutex);
            mRenderCaches.clear();
        }
        
        //! @brief Returns true if a cache has been touched in this manager.
        bool isCacheTouched(const RenderCachePtr < T >& cache) const {
            const PerRendererCacheInfos < T >& infos = findInfos(cache);
            return infos.isTouched();
        }
        
        //! @brief Returns true if any cache is touched.
        bool isAnyCacheTouched() const {
            std::lock_guard l(mMutex);
            
            for (const PerRendererCacheInfos < T >& infos : mRenderCaches)
            {
                if (infos.isTouched())
                    return true;
            }
            
            return false;
        }
        
        //! @brief Returns the infos for a cache.
        PerRendererCacheInfos < T >& findInfos(const RenderCachePtr < T >& cache) {
            std::lock_guard l(mMutex);
            
            for (PerRendererCacheInfos < T >& infos : mRenderCaches)
            {
                if (infos.renderCache == cache)
                    return infos;
            }
            
            throw PerRendererCacheNoInfosFound("PerRendererCache", "findInfos", "Infos for cache %i not found.",
                                               reinterpret_cast < std::uintptr_t >(cache.get()));
        }
        
        //! @brief Returns the infos for a cache.
        const PerRendererCacheInfos < T >& findInfos(const RenderCachePtr < T >& cache) const {
            std::lock_guard l(mMutex);
            
            for (const PerRendererCacheInfos < T >& infos : mRenderCaches)
            {
                if (infos.renderCache == cache)
                    return infos;
            }
            
            throw PerRendererCacheNoInfosFound("PerRendererCache", "findInfos", "Infos for cache %i not found.",
                                               reinterpret_cast < std::uintptr_t >(cache.get()));
        }
        
        //! @brief Touches the given cache.
        void touchCache(const RenderCachePtr < T >& cache) {
            PerRendererCacheInfos < T >& infos = findInfos(cache);
            infos.touch();
        }
        
        //! @brief Touches all caches.
        void touchAllCaches() {
            std::lock_guard l(mMutex);
            for (PerRendererCacheInfos < T >& infos : mRenderCaches)
                infos.touch();
        }
        
        //! @brief Cleans a given cache.
        void cleanCache(const RenderCachePtr < T >& cache) {
            const PerRendererCacheInfos < T >& infos = findInfos(cache);
            infos.clean();
        }
        
        //! @brief Cleans all caches.
        void cleanAllCaches() {
            std::lock_guard l(mMutex);
            for (const PerRendererCacheInfos < T >& infos : mRenderCaches)
                infos.clean();
        }
    };
    
    template < typename T >
    using PerRenderCachePtr = std::shared_ptr < PerRendererCache < T > >;
}

#endif /* PerRendererCache_h */
