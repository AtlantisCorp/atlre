//
//  RenderSurface.cpp
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#include "RenderSurface.h"

namespace Atl
{
    RenderSurface::RenderSurface(Renderer& renderer, const std::string& name, const Params& params)
    : RenderTarget(renderer), mName(name)
    {
        
    }
    
    std::string RenderSurface::name() const
    {
        std::lock_guard l(mMutex);
        return mName;
    }
}
