//
//  HardwareBuffer.cpp
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#include "HardwareBuffer.h"
#include "Error.h"

namespace Atl
{
    // ------------------------------------------------------------------------------------
    // HardwareBuffer

    HardwareBuffer::HardwareBuffer(const HBT& type)
    : mType(type)
    {

    }

    void HardwareBuffer::setType(const HBT& type)
    {
        mType.store(type);
    }

    HBT HardwareBuffer::type() const
    {
        return mType.load();
    }

    void HardwareBuffer::copy(const HardwareBuffer& rhs)
    {
        HardwareBufferLockGuard l(*this);
        HardwareBufferLockGuardCst ll(rhs);

        const std::size_t _size = rhs.size();
        const void* _data = rhs.data();

        if (!_data)
            throw NullError("HardwareBuffer", "copy", "Null data for HardwareBuffer %i.", rhs.index());

        allocate(_size, _data);
        rhs.undata();
    }

    // ------------------------------------------------------------------------------------
    // MemBuffer

    MemBuffer::MemBuffer(const HBT& type)
    : HardwareBuffer(type)
    {

    }

    MemBuffer::MemBuffer(const std::vector < char >& rhs)
    : mBuffer(rhs)
    {
        
    }
    
    MemBuffer::MemBuffer(const char* buffer, std::size_t sz)
    : mBuffer()
    {
        mBuffer.resize(sz);
        memcpy(&mBuffer[0], buffer, sz);
    }
    
    MemBuffer::MemBuffer(const MemBuffer& rhs)
    : mBuffer(rhs.mBuffer)
    {
        
    }
    
    bool MemBuffer::isMemBuffer() const
    {
        return true;
    }
    
    std::size_t MemBuffer::size() const
    {
        std::lock_guard l(mMutex);
        return mBuffer.size();
    }
    
    void MemBuffer::allocate(const std::size_t& sz, const void* ptr)
    {
        mBuffer.resize(sz);

        if (ptr)
            memcpy(&mBuffer[0], ptr, sz);
    }
    
    void MemBuffer::lock() const
    {
        mMutex.lock();
    }
    
    void MemBuffer::unlock() const
    {
        mMutex.unlock();
    }
    
    void* MemBuffer::data()
    {
        return &mBuffer[0];
    }
    
    const void* MemBuffer::data() const
    {
        return &mBuffer[0];
    }
    
    void MemBuffer::undata() const
    {
        /* unused */
    }
    
    HardwareBufferPtr MemBuffer::copy() const
    {
        HardwareBuffer* buffer = (HardwareBuffer*) new MemBuffer(*this);
        return HardwareBufferPtr(buffer);
    }

    std::uint64_t MemBuffer::index() const
    {
        return MakeUniqueIndex::index();
    }
}
