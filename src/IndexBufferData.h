//
//  IndexBufferData.h
//  atl
//
//  Created by jacques tronconi on 30/03/2020.
//

#ifndef ATL_INDEXBUFFERDATA_H
#define ATL_INDEXBUFFERDATA_H

#include "Platform.h"
#include "HardwareBuffer.h"

namespace Atl
{
    //! @brief Enumerates the possible indexes type.
    enum class IndexType 
    {
        UChar, UShort, UInt
    };

    //! @brief Shortcut for IndexType.
    using IT = IndexType;

    //! @brief Defines a data set for indexes.
    class EXPORTED IndexBufferData
    {
        //! @brief The number of elements in this data set.
        std::atomic < std::size_t > mElementsCount;

        //! @brief The index buffer.
        HardwareBufferPtr mBuffer;

        //! @brief The element's type in the data set. The default value of
        //! this is \ref IndexType::UInt.
        std::atomic < IndexType > mType;

    public:
        ATL_SHAREABLE(IndexBufferData)

        //! @brief Constructs a new IndexBufferData.
        IndexBufferData();

        //! @brief Constructs a new IndexBufferData.
        //! @param elementsCount The number of elements in this data set.
        //! @param buffer The buffer where lie the data set.
        //! @param type The indexes type in this data set.
        IndexBufferData(std::size_t elementsCount, const HardwareBufferPtr& buffer, IndexType type);

        //! @brief Sets \ref mElementsCount.
        void setElementsCount(std::size_t elementsCount);

        //! @brief Returns \ref mElementsCount.
        std::size_t elementsCount() const;

        //! @brief Sets \ref mBuffer.
        void setBuffer(const HardwareBufferPtr& buffer);

        //! @brief Returns \ref mBuffer.
        HardwareBufferPtr buffer() const;

        //! @brief Sets \ref mType.
        void setType(const IndexType& type);

        //! @brief Returns \ref mType.
        IndexType type() const;
    };

    //! @brief Defines a Pointer to the \ref IndexBufferData.
    typedef std::shared_ptr < IndexBufferData > IndexBufferDataPtr;
}

#endif // ATL_INDEXBUFFERDATA_H