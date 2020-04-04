//
//  MemoryPool.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef MemoryPool_h
#define MemoryPool_h

#include "Platform.h"
#include "Emitter.h"

#include <atomic>
#include <cstdint>

namespace Atl
{
    class MemoryPool;
    
    //! @brief Defines a listener for some basic MemoryPool events.
    struct MemoryPoolListener : virtual public ListenerBase
    {
        //! @brief Destructor.
        virtual ~MemoryPoolListener() = default;
        //! @brief Launched when memory is running 'low'. Determine the Low Profile in your 
        //! GlobalSettings. 
        virtual void onMemoryLow(MemoryPool&){}
    };

    //! @brief Defines a lock-free Memory Pool size observer.
    class EXPORTED MemoryPool : public Emitter
    {
        //! @brief The currently used size.
        std::atomic < std::size_t > mCurrSize;
        
        //! @brief The maximum size allowed in this pool.
        std::atomic < std::size_t > mMaxSize;
        
    public:
        //! @brief Listener for this class.
        typedef MemoryPoolListener Listener;
        
        //! @brief Constructs a new memory pool.
        MemoryPool(std::size_t maxSize = 0);
        
        //! @brief Changes the size used by an object in this pool.
        //! @param oldsz The size previously used by this object. If zero, it means
        //! the object has been constructed.
        //! @param newsz The size the object would like to use. If zero, it means
        //! the object is being destroyed.
        //! The currently used size is updated in consequence: if the size is growing,
        //! an assertion is made with the maximum size in this pool. If the size
        //! is minimizing, then the current size is only updated without assertion.
        void change(std::size_t oldsz, std::size_t newsz);
        
        //! @brief Returns true if the delta size between the old and new size of
        //! an object is available in this memory pool.
        //! @note If the new size is inferior to the old size, it is assumed the
        //! old size was already in this pool and thus, returns true.
        bool isAvailable(std::size_t oldsz, std::size_t newsz) const;
        
        //! @brief Returns the currently used size, in bytes.
        inline std::size_t currentSize() const { return mCurrSize; }
        
        //! @brief Returns the maximum size of this pool, in bytes.
        inline std::size_t maxSize() const { return mMaxSize; }
        
        //! @brief Returns the available size in this pool, in bytes.
        inline std::size_t availableSize() const { return mMaxSize - mCurrSize; }
    };
}

#endif /* MemoryPool_h */
