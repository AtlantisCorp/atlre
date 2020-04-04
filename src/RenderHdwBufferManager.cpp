//
//  RenderHdwBufferManager.cpp
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#include "RenderHdwBufferManager.h"

namespace Atl
{
    namespace details
    {
        //! @brief A Map that tell for each HardwareBufferType, the correct
        //! RenderHdwBuffer class to instantiate. As seen on stack overflow, we
        //! cannot have a typical std::type_index as value of a map as it doesn't
        //! have a default constructor. A solution is to wrap it into std::optional.
        //! \see https://stackoverflow.com/a/46199217
        static std::map < HBT, std::optional < std::type_index > > HBTToTIdx =
        {
            { HBT::Vertex,  typeid(RenderHdwVertexBuffer) },
            { HBT::Index,   typeid(RenderHdwIndexBuffer) },
            { HBT::Misc,    typeid(RenderHdwBuffer) }
        };
    }

    RenderHdwBufferManager::ThisRenderHdwBufferObserver::ThisRenderHdwBufferObserver(RenderHdwBufferManager& rhs)
    : mManager(rhs)
    {
        
    }
    
    bool RenderHdwBufferManager::ThisRenderHdwBufferObserver::isAvailable(std::size_t oldsz, std::size_t newsz) const
    {
        return mManager.mPool.isAvailable(oldsz, newsz);
    }
    
    void RenderHdwBufferManager::ThisRenderHdwBufferObserver::change(std::size_t oldsz, std::size_t newsz)
    {
        mManager.mPool.change(oldsz, newsz);
    }
    
    RenderHdwBufferManager::RenderHdwBufferManager(Renderer& rhs, std::size_t maxSize)
    : RenderObjectManager(rhs), mPool(maxSize)
    , mObserver(std::make_shared < ThisRenderHdwBufferObserver >(*this))
    {
        
    }
    
    RenderHdwBufferPtr RenderHdwBufferManager::make(const std::type_index& type)
    {
        return mFactory.construct(type, renderer(), mObserver);
    }
    
    void RenderHdwBufferManager::add(const RenderHdwBufferPtr& buffer)
    {
        RenderObjectManager::add(buffer);
    }
    
    bool RenderHdwBufferManager::isSizeAvailable(std::size_t sz) const
    {
        return mPool.isAvailable(0, sz);
    }

    RenderHdwBufferPtr RenderHdwBufferManager::findRelated(const MemBuffer::Index& index) const
    {
        std::lock_guard l(mMutex);

        for (const RenderHdwBufferPtr& buffer : mObjects)
        {
            if (!buffer)
                throw NullError("RenderHdwBufferManager", "findRelated", "Null RenderHdwBuffer found.");
            
            if (buffer->relatedIndex() == index)
                return buffer;
        }

        return nullptr;
    }

    RenderHdwBufferPtr RenderHdwBufferManager::findOrCreateRelated(const MemBufferPtr& buffer)
    {
        if (!buffer)
            throw NullError("RenderHdwBufferManager", "findOrCreateRelated", "Null MemBuffer.");

        RenderHdwBufferPtr hdwBuffer = findRelated(buffer->index());

        if (!hdwBuffer)
        {
            const std::size_t sizeNeeded = buffer->size();

            if (!isSizeAvailable(sizeNeeded))
                throw NotEnoughMemory("RenderHdwBufferManager", "findOrCreateRelated", "Memory limit reached for %i bytes.", sizeNeeded);

            HardwareBufferLockGuard l(*buffer);
            
            std::optional < std::type_index > buffType = details::HBTToTIdx[buffer->type()];
            if (!buffType.has_value())
                throw RenderHdwBufferTypeNotSupported("RenderHdwBufferManager", "findOrCreateRelated", "HardwareBuffer type %i is not supported.", static_cast < int >(buffer->type()));
            
            hdwBuffer = make(buffType.value());
            hdwBuffer->allocate(sizeNeeded, buffer->data());
            buffer->undata();

            add(hdwBuffer);
        }

        return hdwBuffer;
    }

    RenderHdwBufferPtr RenderHdwBufferManager::copy(const RenderHdwBufferPtr& buffer)
    {
        if (!buffer)
            throw NullError("RenderHdwBufferManager", "copy", "Null RenderHdwBuffer.");

        const std::size_t sizeNeeded = buffer->size();

        if (!isSizeAvailable(sizeNeeded))
                throw NotEnoughMemory("RenderHdwBufferManager", "copy", "Memory limit reached for %i bytes.", sizeNeeded);
        
        HardwareBufferLockGuard l(*buffer);
        
        std::optional < std::type_index > buffType = details::HBTToTIdx[buffer->type()];
        if (!buffType.has_value())
            throw RenderHdwBufferTypeNotSupported("RenderHdwBufferManager", "copy", "HardwareBuffer type %i is not supported.", static_cast < int >(buffer->type()));
        
        RenderHdwBufferPtr hdwBuffer = make(buffType.value());
        hdwBuffer->allocate(sizeNeeded, buffer->data());
        buffer->undata();

        add(hdwBuffer);
        return hdwBuffer;
    }

    void RenderHdwBufferManager::removeUnusedBuffers()
    {
        RenderHdwBufferList buffers;
        { 
            std::lock_guard l(mMutex);
            buffers = mObjects;
        }

        for (RenderHdwBufferPtr& buffer : buffers)
        {
            if (buffer.use_count() == 2) // This one plus the object stored.
                remove(buffer);
        }
    }

    void RenderHdwBufferManager::onMemoryLow(MemoryPool&)
    {
        if (mTriesFreeOnLow)
            removeUnusedBuffers();
    }
}
