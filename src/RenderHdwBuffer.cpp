//
//  RenderHdwBuffer.cpp
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#include "RenderHdwBuffer.h"

namespace Atl
{
    // ------------------------------------------------------------------------------------
    // RenderHdwBuffer

    RenderHdwBuffer::RenderHdwBuffer(Renderer& rhs, const RenderHdwBufferObserverPtr& observer, const HBT& type)
    : HardwareBuffer(type), RenderObject(rhs), mObserver(observer), mRelatedIndex(0)
    {
        if (!observer)
            throw NullError("RenderHdwBuffer", "RenderHdwBuffer", "Null observer passed.");
    }

    MemBuffer::Index RenderHdwBuffer::relatedIndex() const
    {
        return mRelatedIndex.load();
    }

    void RenderHdwBuffer::setRelatedIndex(const MemBuffer::Index& relIndex) 
    {
        mRelatedIndex.store(relIndex);
    }

    // ------------------------------------------------------------------------------------
    // RenderHdwVertexBuffer

    RenderHdwVertexBuffer::RenderHdwVertexBuffer(Renderer& rhs, const RenderHdwBufferObserverPtr& observer)
    : RenderHdwBuffer(rhs, observer, HBT::Vertex)
    {

    }

    // ------------------------------------------------------------------------------------
    // RenderHdwIndexBuffer

    RenderHdwIndexBuffer::RenderHdwIndexBuffer(Renderer& rhs, const RenderHdwBufferObserverPtr& observer)
    : RenderHdwBuffer(rhs, observer, HBT::Index)
    {
        
    }
}
