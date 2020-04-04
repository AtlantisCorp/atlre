//
//  MemoryPool.cpp
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#include "MemoryPool.h"
#include "Error.h"
#include <any>

namespace Atl
{
    MemoryPool::MemoryPool(std::size_t maxSize)
    : mCurrSize(0), mMaxSize(0)
    {
        
    }
    
    void MemoryPool::change(std::size_t oldsz, std::size_t newsz)
    {
        if (!newsz)
            mCurrSize -= oldsz;
        
        if (oldsz < newsz)
        {
            const std::size_t delta = newsz - oldsz;
            if (mCurrSize + delta > mMaxSize && mMaxSize > 0)
                throw NotEnoughMemory("MemoryPool", "change", "Memory limit of %i bytes exceeded. (%i)",
                                      mMaxSize.load(), mCurrSize + delta);
            mCurrSize = mCurrSize + delta;

            if (/*Settings::Get().isSet("mLowProfile") &&*/ mMaxSize > 0)
            {
                const float lowProfile = 80.0f; // std::any_cast < float >(Settings::Get().get("mLowProfile"));
                const float currProfile = (float)mCurrSize.load() / (float)mMaxSize.load();

                if (currProfile > lowProfile)
                    send(&Listener::onMemoryLow, *this);
            }
        }
        
        else
        {
            const std::size_t delta = oldsz - newsz;
            mCurrSize = mCurrSize - delta;
        }
    }
    
    bool MemoryPool::isAvailable(std::size_t oldsz, std::size_t newsz) const
    {
        if (oldsz >= newsz)
            return true;
        
        if (!mMaxSize)
            return true;
        
        return mCurrSize + (newsz - oldsz) <= mMaxSize;
    }
}
