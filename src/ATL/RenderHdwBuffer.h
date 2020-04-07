//
//  RenderHdwBuffer.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef RenderHdwBuffer_h
#define RenderHdwBuffer_h

#include "HardwareBuffer.h"
#include "RenderHdwBufferObserver.h"
#include "Factory.h"
#include "RenderObject.h"

#include <typeindex>

namespace Atl
{
    //! @brief Defines a base class for all Render Hardware Buffers.
    class EXPORTED RenderHdwBuffer : public HardwareBuffer,
                                     public RenderObject
    {
        //! @brief The observer for size changes.
        RenderHdwBufferObserverPtr mObserver;

        //! @brief The related index from a MemBuffer.
        //! This index is set by RenderHdwBufferManager to relationate this buffer to a
        //! MemBuffer. The MemBuffer can then retrieve the buffer in the manager, with
        //! \ref RenderHdwBufferManager::findOrCreateRelated().
        std::atomic < MemBuffer::Index > mRelatedIndex;
        
    public:
        //! @brief Constructs an empty buffer.
        //! @param rhs The \ref Renderer that constructs this buffer.
        //! @param observer The Observer to register into this buffer.
        //! @param type The \ref HardwareBufferType related to this buffer.
        RenderHdwBuffer(Renderer& rhs, const RenderHdwBufferObserverPtr& observer, const HBT& type = HBT::Misc);

        //! @brief Destructor.
        virtual ~RenderHdwBuffer() = default;

        //! @brief Returns \ref mRelatedIndex.
        MemBuffer::Index relatedIndex() const;

        //! @brief Sets \ref mRelatedIndex.
        void setRelatedIndex(const MemBuffer::Index& relIndex);
    };
    
    typedef std::shared_ptr < RenderHdwBuffer > RenderHdwBufferPtr;
    typedef std::vector < RenderHdwBufferPtr > RenderHdwBufferList;
    
    using RenderHdwBufferFactory = Factory <
    std::type_index,
    RenderHdwBufferPtr,
    Renderer&,
    const RenderHdwBufferObserverPtr& >;

    //! @brief Defines a Vertex Buffer.
    struct EXPORTED RenderHdwVertexBuffer : public RenderHdwBuffer 
    {
        //! @brief Constructs a RenderHdwBuffer where type is HBT::Vertex.
        RenderHdwVertexBuffer(Renderer& rhs, const RenderHdwBufferObserverPtr& observer);
    };

    //! @brief Defines an Index Buffer.
    struct EXPORTED RenderHdwIndexBuffer : public RenderHdwBuffer
    {
        //! @brief Constructs a RenderHdwBuffer where type is HBT::Index.
        RenderHdwIndexBuffer(Renderer& rhs, const RenderHdwBufferObserverPtr& observer);
    };
}

#endif /* RenderHdwBuffer_h */
