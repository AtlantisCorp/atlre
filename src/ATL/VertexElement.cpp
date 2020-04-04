//
//  VertexElement.cpp
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#include "VertexElement.h"
#include "Error.h"

namespace Atl
{
    namespace Details
    {
        static EXPORTED std::size_t sizeForType[static_cast<unsigned>(VertexElementType::Max)] =
        {
            sizeof(float) * 1,
            sizeof(float) * 2,
            sizeof(float) * 3,
            sizeof(float) * 4,
            
            sizeof(double) * 1,
            sizeof(double) * 2,
            sizeof(double) * 3,
            sizeof(double) * 4,
            
            sizeof(short) * 1,
            sizeof(short) * 2,
            sizeof(short) * 3,
            sizeof(short) * 4,
            
            sizeof(unsigned short) * 1,
            sizeof(unsigned short) * 2,
            sizeof(unsigned short) * 3,
            sizeof(unsigned short) * 4,
            
            sizeof(int) * 1,
            sizeof(int) * 2,
            sizeof(int) * 3,
            sizeof(int) * 4,
            
            sizeof(unsigned int) * 1,
            sizeof(unsigned int) * 2,
            sizeof(unsigned int) * 3,
            sizeof(unsigned int) * 4,
            
            sizeof(int8_t) * 4,
            sizeof(uint8_t) * 4,
            sizeof(short) * 2,
            sizeof(short) * 4,
            sizeof(unsigned short) * 2,
            sizeof(unsigned short) * 4
        };
        
        static EXPORTED std::size_t SizeForType(VertexElementType type)
        {
            const unsigned idx = static_cast < unsigned >(type);
            if (idx > static_cast < unsigned >(VertexElementType::Max))
                throw OutOfRange("Atl::Details", "SizeForType", "Index %i is out of range.", idx);
            return sizeForType[idx];
        }
    }
    
    VertexElement::VertexElement()
    : mSource(0), mOffset(0), mType(VertexElementType::Float1), mMeaning()
    {
        
    }
    
    VertexElement::VertexElement(unsigned short source, std::size_t offset, VertexElementType type, const std::string& meaning)
    : mSource(source), mOffset(offset), mType(type), mMeaning(meaning)
    {
        
    }
    
    unsigned short VertexElement::source() const
    {
        return mSource;
    }
    
    std::size_t VertexElement::offset() const
    {
        return mOffset;
    }
    
    VertexElementType VertexElement::type() const
    {
        return mType;
    }
    
    const std::string& VertexElement::meaning() const
    {
        return mMeaning;
    }
    
    std::size_t VertexElement::size() const
    {
        return Details::SizeForType(mType);
    }
}
