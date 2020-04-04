//
//  RenderObject.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef RenderObject_h
#define RenderObject_h

#include "Platform.h"

namespace Atl
{
    class Renderer;
    
    //! @brief Base class for any object managed and created by the Renderer.
    //! All Classes that are fully managed by the renderer should derive from this
    //! class.
    class EXPORTED RenderObject
    {
        //! @brief The object's renderer.
        Renderer& mRenderer;
        
    public:
        
        //! @brief Constructs an object.
        RenderObject(Renderer& rhs);
        
        //! @brief Returns the renderer that owns this object.
        Renderer& renderer();
        
        //! @brief Returns the renderer that owns this object.
        const Renderer& renderer() const;
    };
}

#endif /* RenderObject_h */
