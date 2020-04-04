//
//  RenderSurface.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef RenderSurface_h
#define RenderSurface_h

#include "RenderTarget.h"
#include "Factory.h"
#include "RenderObjectManager.h"
#include "Params.h"

namespace Atl
{
    //! @brief Interface for all surfaces that may be used as a RenderTarget.
    //! A RenderSurface is not always a RenderWindow: it may be an offscreen surface,
    //! or even a surface in another native window where we can draw into.
    class EXPORTED RenderSurface : public RenderTarget
    {
        mutable std::mutex mMutex;
        
        //! @brief The surface's name.
        std::string mName;
        
    public:
        //! @brief Default constructor.
        //! @param renderer The Renderer that constructs this surface.
        //! @param name The name given to this surface.
        //! @param params The Params structure used to construct the surface. Notes
        //! this last parameter is used only in derived classes.
        RenderSurface(Renderer& renderer, const std::string& name, const Params& params);
        
        //! @brief Default destructor.
        virtual ~RenderSurface() = default;
        
        //! @brief Returns true if the surface is closed.
        virtual bool isClosed() const = 0;
        
        //! @brief Updates the surface.
        virtual void update() const = 0;
        
        //! @brief Returns the surface's size, in pixels.
        virtual glm::ivec2 size() const = 0;
        
        //! @brief Returns the surface's name.
        std::string name() const;
    };
    
    typedef std::shared_ptr < RenderSurface > RenderSurfacePtr;
    typedef std::vector < RenderSurfacePtr > RenderSurfaceList;
    
    using RenderSurfaceFactory = Factory <
    std::string,
    RenderSurfacePtr,
    Renderer&,
    const std::string&,
    const Params& >;
    
    using RenderSurfaceManager = RenderObjectManager <
    RenderSurface,
    RenderSurfaceFactory >;
}

#endif /* RenderSurface_h */
