//
//  Touchable.cpp
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#include "Touchable.h"

namespace Atl
{
    TimeTouchable::TimeTouchable()
    : mLastTouch(Clock::now()), mLastClean(Clock::now())
    {
        
    }
    
    TimeTouchable::TimeTouchable(const TimeTouchable&)
    : TimeTouchable()
    {
        
    }
    
    TimeTouchable& TimeTouchable::operator = (const TimeTouchable& rhs)
    {
        mLastClean = rhs.mLastClean;
        mLastTouch = rhs.mLastTouch;
        return *this;
    }
    
    bool TimeTouchable::isTouched() const
    {
        std::lock_guard l(mMutex);
        return mLastClean < mLastTouch;
    }
    
    void TimeTouchable::touch()
    {
        std::lock_guard l(mMutex);
        mLastTouch = Clock::now();
    }
    
    void TimeTouchable::clean() const
    {
        std::lock_guard l(mMutex);
        mLastClean = Clock::now();
    }
}
