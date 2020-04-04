//
//  RenderWindow.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef RenderWindow_h
#define RenderWindow_h

#include "RenderSurface.h"

namespace Atl
{
    enum RenderWindowStyle
    {
        RWSTitled         = 0b00000001,
        RWSClosable       = 0b00000010,
        RWSResizable      = 0b00000100,
        RWSMiniaturizable = 0b00001000,
        RWSFullscreen     = 0b00010000,
        
        RWSDefault        = 0b00001111
    };
    
    //! @brief Interface for a native rendering window.
    class EXPORTED RenderWindow : public RenderSurface
    {
    public:

        //! @brief Flushes the internal Framebuffer for this Window.
        //! This should be called after a frame has been rendered. Typically
        //! this happens after \ref Renderer::render() finishes its job.
        virtual void flush() = 0;

        //! @brief Shows the window on screen.
        virtual void show() = 0;
    };
    
    typedef std::shared_ptr < RenderWindow > RenderWindowPtr;
}

#endif /* RenderWindow_h */
