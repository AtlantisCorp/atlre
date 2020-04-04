//
//  FrameCounter.cpp
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#include "FrameCounter.h"

namespace Atl
{
    FrameCounter::FrameCounter()
    {
        mLastFrameTime = HighResClock::now();
        mAverageFrameTime = std::chrono::milliseconds::zero();
        mFrameCount = 0;
    }
    
    void FrameCounter::add()
    {
        using namespace std::chrono;
        
        const auto nowTime = HighResClock::now();
        const auto lastFrameDuration = duration_cast < milliseconds >(nowTime - mLastFrameTime);
        const milliseconds a = lastFrameDuration - mAverageFrameTime;
        const milliseconds b = a / mFrameCount;
        mAverageFrameTime = mAverageFrameTime + b;
        
        mFrameCount += 1;
        mLastFrameTime = HighResClock::now();
    }
    
    float FrameCounter::secondsSinceLastFrame() const
    {
        return ((float)durationSinceLastFrame<std::chrono::milliseconds>().count()) / 1000.0f;
    }
    
    std::chrono::milliseconds FrameCounter::averageFrameTime() const
    {
        return mAverageFrameTime;
    }
}
