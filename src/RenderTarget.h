//
//  RenderTarget.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef RenderTarget_h
#define RenderTarget_h

#include "RenderObject.h"

namespace Atl
{
    //! @brief Interface for all rendering target.
    //! A RenderTarget is an object that can be used (with \ref bind()) to draw RenderCommands
    //! in itself. The Renderer use it with \ref Renderer::render() to draw a RenderCommand.
    class EXPORTED RenderTarget : public RenderObject
    {
    public:
        using RenderObject::RenderObject;
        
        //! @brief Default destructor.
        virtual ~RenderTarget() = default;
        
        //! @brief Binds the render target for use.
        virtual void bind() = 0;
    };
}

#endif /* RenderTarget_h */
