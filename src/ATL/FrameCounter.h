//
//  FrameCounter.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef FrameCounter_h
#define FrameCounter_h

#include "Platform.h"

#include <chrono>
#include <cstdint>

namespace Atl
{
    typedef std::chrono::high_resolution_clock HighResClock;
    typedef HighResClock::time_point HighResTime;
    
    //! @brief Helper for counting frames and some frame time stats.
    class EXPORTED FrameCounter
    {
        HighResTime mLastFrameTime;
        std::uint64_t mFrameCount;
        std::chrono::milliseconds mAverageFrameTime;
        
    public:
        
        FrameCounter();
        
        void add();
        
        float secondsSinceLastFrame() const;
        
        template < typename T >
        T durationSinceLastFrame() const
        {
            using namespace std::chrono;
            return duration_cast < T >(HighResClock::now() - mLastFrameTime);
        }
        
        std::chrono::milliseconds averageFrameTime() const;
    };
}

#endif /* FrameCounter_h */
