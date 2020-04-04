//
//  Renderable.h
//  atl
//
//  Created by jacques tronconi on 21/03/2020.
//

#ifndef Renderable_h
#define Renderable_h

#include "Platform.h"
#include "Emitter.h"
#include "Error.h"

#include <future>

namespace Atl
{
    class RenderCommand;
    class Renderer;
    class Renderable;

    //! @brief Launched Error when a Renderer is invalid to render a Renderable.
    struct EXPORTED RenderableInvalidRenderer : public Error
    { using Error::Error; };

    //! @brief A default listener for Renderable.
    class EXPORTED RenderableListener : virtual public ListenerBase
    {
    public:
        //! @brief Destructor.
        virtual ~RenderableListener() = default;

        //! @brief Launched when a \ref Renderable will be rendered.
        virtual void onRenderableWillRender(const Renderable&, RenderCommand&){}

        //! @brief Launched when a \ref Renderable did render.
        virtual void onRenderableDidRender(const Renderable&, RenderCommand&){}

        //! @brief Launched when a \ref Renderable will build.
        virtual void onRenderableWillBuild(Renderable&, Renderer&){}

        //! @brief Launched when a \ref Renderable did build.
        virtual void onRenderableDidBuild(Renderable&, Renderer&){}
    };
    
    //! @brief An interface for all objects that can fill a RenderCommand.
    class EXPORTED Renderable : virtual public Emitter
    {
    public:
        //! @brief The renderable's listener.
        typedef RenderableListener Listener;

        //! @brief Default destructor.
        virtual ~Renderable() = default;
        
        //! @brief Should renders the object into the RenderCommand.
        //! The RenderCommand must be filled accordingly, for the Renderer to be able
        //! to draw this object. This function can be used in another thread as the
        //! rendering thread, because no drawing API call are involved in this function.
        virtual std::future < void > render(RenderCommand& to) const = 0;
        
        //! @brief Should build the object to be ready for drawing.
        //! This function should creates all intermediary objects needed to fill correctly
        //! the RenderCommand provided by the given Renderer.
        virtual std::future < void > build(Renderer& rhs) = 0;
        
        //! @brief Returns, in bytes, the memory used on the GPU RAM for this cache and for
        //! the given renderer.
        virtual std::size_t size(Renderer&) const = 0;
    };

    //! @brief A Generic Renderable Pointer.
    typedef std::shared_ptr < Renderable > RenderablePtr;
    //! @brief Defines a list of shared Renderable.
    typedef std::vector < RenderablePtr > RenderableList;
}

#endif /* Renderable_h */
