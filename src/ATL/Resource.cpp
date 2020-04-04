//
//  Resource.cpp
//  atlre
//
//  Created by jacques tronconi on 10/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "Resource.h"
#include <sstream>

namespace Atl
{
    Resource::Resource(const std::string& name)
    : mName(name), mState(0)
    {
        
    }
    
    unsigned Resource::state() const
    {
        return mState.load();
    }
    
    std::string Resource::name() const
    {
        std::lock_guard l(mMutex);
        return mName;
    }
    
    bool Resource::isLoaded() const
    {
        return mState.load() & Loaded;
    }
    
    bool Resource::isCached() const
    {
        return mState.load() & Cached;
    }
    
    void Resource::setState(unsigned state)
    {
        mState.store(state);
    }
}
