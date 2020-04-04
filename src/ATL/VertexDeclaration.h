//
//  VertexDeclaration.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef VertexDeclaration_h
#define VertexDeclaration_h

#include "Platform.h"
#include "VertexElement.h"

namespace Atl
{
    //! @brief Defines a list of multiple VertexElements.
    //! The VertexDeclaration is used to bind multiple HardwareBuffers to a vertex
    //! program stage. It describes all the elements are organized in those buffers,
    //! and which should be used to draw something.
    //! @warning This class is not thread-safe. You should always use this class in
    //! your main render thread.
    class EXPORTED VertexDeclaration
    {
        //! @brief The list of VertexElements.
        VertexElementList mElements;
        
    public:
        ATL_SHAREABLE(VertexDeclaration)
        
        //! @brief Default constructor.
        VertexDeclaration() = default;
        
        //! @brief Adds an element to this declaration.
        //! @param source The binding from which we should take the HardwareBuffer for the
        //! created element.
        //! @param offset The offset where the element's list actually starts in the bound
        //! buffer.
        //! @param type The type that describe this element.
        //! @param meaning The element's meaning.
        //! @return A reference to the constructed VertexElement.
        const VertexElement& addElement(unsigned short source, std::size_t offset, VertexElementType type, const std::string& meaning);
        
        //! @brief Adds a single element at the given position.
        //! @param index The position where to insert the element.
        //! @param source The binding from which we should take the HardwareBuffer for the
        //! created element.
        //! @param offset The offset where the element's list actually starts in the bound
        //! buffer.
        //! @param type The type that describe this element.
        //! @param meaning The element's meaning.
        //! @return A reference to the constructed VertexElement.
        //! @throw An OutOfRange Error if index is superior to \ref elementCount().
        const VertexElement& insertElement(unsigned index, unsigned short source, std::size_t offset, VertexElementType type, const std::string& meaning);
        
        //! @brief Sets the VertexElement at given index.
        //! @param index The position where to set the element.
        //! @param source The binding from which we should take the HardwareBuffer for the
        //! created element.
        //! @param offset The offset where the element's list actually starts in the bound
        //! buffer.
        //! @param type The type that describe this element.
        //! @param meaning The element's meaning.
        //! @return A reference to the constructed VertexElement.
        //! @throw An OutOfRange Error if index is superior or equal to \ref elementCount().
        const VertexElement& setElement(unsigned index, unsigned short source, std::size_t offset, VertexElementType type, const std::string& meaning);
        
        //! @brief Removes an element at given position.
        //! @param index The position where to remove the element.
        void removeElement(unsigned index);
        
        //! @brief Removes an element.
        //! @param meaning The meaning of the element. If two elements have the same
        //! meaning, the first element encountered is removed.
        void removeElement(const std::string& meaning);
        
        //! @brief Removes all elements.
        void removeAllElements();
        
        //! @brief Returns an element.
        //! @param index The element's index in the list.
        //! @throw An OutOfRange Error if index is invalid.
        const VertexElement& findElement(unsigned index) const;
        
        //! @brief Returns an element.
        //! @param meaning The element's meaning. If multiple elements have the same
        //! meaning, only the first one is returned.
        //! @throw An OutOfRange Error if meaning is invalid.
        const VertexElement& findElement(const std::string& meaning) const;
        
        //! @brief Returns the elements for a buffer source.
        //! @param source The buffer source to inspect.
        VertexElementList findElementsForSource(unsigned short source) const;
        
        //! @brief Returns the total size of this vertex declaration, in bytes.
        std::size_t vertexSize() const;
        
        //! @brief Returns the total size of this vertex declaration for a single
        //! buffer source, in bytes.
        //! @param source The buffer source to inspect.
        std::size_t vertexSizeForSource(unsigned short source) const;
        
        //! @brief Returns the number of elements in this declaration.
        std::size_t elementsCount() const;
    };
    
    typedef std::shared_ptr < VertexDeclaration > VertexDeclarationPtr;
}

#endif /* VertexDeclaration_h */
