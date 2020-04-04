//
//  RenderHdwBufferObserver.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef RenderHdwBufferObserver_h
#define RenderHdwBufferObserver_h

#include <cstdint>
#include <memory>

namespace Atl
{
    //! @brief An interface for an observer on the size of a RenderHdwBuffer.
    //! This interface is merely used by RenderHdwBufferManager to follow the size changes
    //! of its buffers. A buffer can use this observer to know if a new size is available,
    //! for example when allocating more size. The RenderHdwBuffer must also notifiate
    //! its size changes with \ref change() function.
    class RenderHdwBufferObserver
    {
    public:
        //! @brief Default destructor.
        virtual ~RenderHdwBufferObserver() = default;
        
        //! @brief Returns true if the new size is available from the old size, in bytes.
        virtual bool isAvailable(std::size_t oldsz, std::size_t newsz) const = 0;
        
        //! @brief Notifiates a size change from oldsz to newsz, in bytes.
        virtual void change(std::size_t oldsz, std::size_t newsz) = 0;
    };
    
    typedef std::shared_ptr < RenderHdwBufferObserver > RenderHdwBufferObserverPtr;
}

#endif /* RenderHdwBufferObserver_h */
