//
//  IndexBufferData.cpp
//  atl
//
//  Created by jacques tronconi on 30/03/2020.
//

#include "IndexBufferData.h"

namespace Atl
{
    IndexBufferData::IndexBufferData()
    : mElementsCount(0), mBuffer(nullptr), mType(IT::UInt)
    {

    }

    IndexBufferData::IndexBufferData(std::size_t elementsCount, const HardwareBufferPtr& buffer, IndexType type)
    : mElementsCount(elementsCount), mBuffer(buffer), mType(type)
    {

    }

    void IndexBufferData::setElementsCount(std::size_t elementsCount)
    {
        mElementsCount.store(elementsCount);
    }

    std::size_t IndexBufferData::elementsCount() const
    {
        return mElementsCount.load();
    }

    void IndexBufferData::setBuffer(const HardwareBufferPtr& buffer)
    {
        std::atomic_store(&mBuffer, buffer);
    }

    HardwareBufferPtr IndexBufferData::buffer() const
    {
        return std::atomic_load(&mBuffer);
    }

    void IndexBufferData::setType(const IndexType& type)
    {
        mType.store(type);
    }

    IndexType IndexBufferData::type() const
    {
        return mType.load();
    }
}
