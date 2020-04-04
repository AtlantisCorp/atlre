//
//  VertexElement.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef VertexElement_h
#define VertexElement_h

#include "Platform.h"

#include <string>
#include <list>

namespace Atl
{
    //! @brief Enumerates the possible types for an element.
    enum class VertexElementType
    {
        Float1 = 0,
        Float2 = 1,
        Float3 = 2,
        Float4 = 4,
        
        Double1 = 5,
        Double2 = 6,
        Double3 = 7,
        Double4 = 8,
        
        Short1 = 9,
        Short2 = 10,
        Short3 = 11,
        Short4 = 12,
        
        UShort1 = 13,
        UShort2 = 14,
        UShort3 = 15,
        UShort4 = 16,
        
        Int1 = 17,
        Int2 = 18,
        Int3 = 19,
        Int4 = 20,
        
        UInt1 = 21,
        UInt2 = 22,
        UInt3 = 23,
        UInt4 = 24,
        
        Byte4Norm = 25,
        UByte4Norm = 26,
        Short2Norm = 27,
        Short4Norm = 28,
        UShort2Norm = 29,
        UShort4Norm = 30,
        
        Max = UShort4Norm
    };
    
    //! @brief Defines a single element in a VertexDeclaration.
    class EXPORTED VertexElement
    {
        //! @brief The index in the VertexBufferBinding where to take data.
        unsigned short mSource;
        
        //! @brief The offset in the buffer where the element start at, in bytes.
        std::size_t mOffset;
        
        //! @brief The element's type.
        VertexElementType mType;
        
        //! @brief The element's meaning. Some default meanings are available, but you can
        //! define any meaning you want.
        std::string mMeaning;
        
    public:
        
        //! @brief Default constructor.
        VertexElement();
        
        //! @brief Constructs a new VertexElement.
        //! @note This function should be call from \ref VertexDeclaration::addElement().
        VertexElement(unsigned short source, std::size_t offset, VertexElementType type, const std::string& meaning);
        
        //! @brief Default copy constructor.
        VertexElement(const VertexElement&) = default;
        
        //! @brief Returns the element's source.
        unsigned short source() const;
        
        //! @brief Returns the element's offset.
        std::size_t offset() const;
        
        //! @brief Returns the element's type.
        VertexElementType type() const;
        
        //! @brief Returns the element's meaning.
        const std::string& meaning() const;
        
        //! @brief Returns the size of this element, in bytes.
        std::size_t size() const;
    };
    
    typedef std::list < VertexElement > VertexElementList;
}

#endif /* VertexElement_h */
