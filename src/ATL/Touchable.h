//
//  Touchable.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef Touchable_h
#define Touchable_h

#include "Platform.h"

#include <chrono>
#include <mutex>

namespace Atl
{
    //! @brief An interface for a class that can be 'touched' and then 'cleaned'.
    //! This is the same system as a 'dirty' state, generally used to maintain wether
    //! an object has been modified or not since last 'clean'.
    class EXPORTED Touchable
    {
    public:
        //! @brief Default destructor.
        virtual ~Touchable() = default;
        
        //! @brief Returns true if this object has been touched.
        virtual bool isTouched() const = 0;
        
        //! @brief Touches the object to signal it has been modified.
        virtual void touch() = 0;
        
        //! @brief Notifiates the object is no more in a 'modified' state.
        virtual void clean() const = 0;
    };
    
    //! @brief A default implementation of a Touchable with std::chrono.
    class EXPORTED TimeTouchable : public Touchable
    {
        typedef std::chrono::high_resolution_clock Clock;
        typedef Clock::time_point ClockTime;
        
        //! @brief The last time this object has been modified.
        ClockTime mLastTouch;
        
        //! @brief The last time it has been cleaned.
        mutable ClockTime mLastClean;
        
        //! @brief Internal mutex.
        mutable std::mutex mMutex;
        
    public:
        //! @brief Default constructor.
        //! Initializes mLastTouch and mLastClean to a Clock::now() state.
        TimeTouchable();
        
        //! @brief Default copy constructor.
        TimeTouchable(const TimeTouchable& rhs);
        
        //! @brief Copy assignment operator.
        TimeTouchable& operator = (const TimeTouchable& rhs);
        
        //! Default destructor.
        ~TimeTouchable() = default;
        
        //! @brief Returns true if mLastTouch > mLastClean.
        bool isTouched() const;
        
        //! @brief Updates mLastTouch to the current clock value.
        void touch();
        
        //! @brief Updates mLastClean to the current clock value.
        void clean() const;
    };
}

#endif /* Touchable_h */
