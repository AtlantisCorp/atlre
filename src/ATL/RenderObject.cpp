//
//  RenderObject.cpp
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#include "RenderObject.h"

namespace Atl
{
    RenderObject::RenderObject(Renderer& rhs)
    : mRenderer(rhs)
    {
        
    }
    
    Renderer& RenderObject::renderer()
    {
        return mRenderer;
    }
    
    const Renderer& RenderObject::renderer() const
    {
        return mRenderer;
    }
}
