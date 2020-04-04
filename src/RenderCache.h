//
//  RenderCache.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef RenderCache_h
#define RenderCache_h

#include "Renderable.h"
#include <memory>

namespace Atl
{
    //! @brief An interface for providing a cache rendering structure for a given
    //! renderer and object (generally this object is a renderable resource, like SubModel).
    template < typename T >
    class RenderCache : public Renderable
    {
    protected:
        //! @brief The renderer for which this renderable has been created.
        Renderer& mRenderer;
        
        //! @brief The object which this renderable is for.
        T& mOwner;
        
    public:
        
        //! @brief Constructs a new cache structure.
        RenderCache(Renderer& renderer, T& owner): mRenderer(renderer), mOwner(owner) {
            
        }
        
        //! @brief Default destructor.
        virtual ~RenderCache() = default;
        
        //! @brief Returns true if this cache is for the given renderer.
        //! The default version compares the address of the two renderers, and returns true
        //! on equality. You may implement another version if you want interoperability between
        //! caches, however you may also have to reimplement derived classes.
        virtual bool isFrom(Renderer& rhs) const {
            return &mRenderer == &rhs;
        }
    };
    
    template < typename T >
    using RenderCachePtr = std::shared_ptr < RenderCache < T > >;
}

#endif /* RenderCache_h */
