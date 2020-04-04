//
//  VertexInfos.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef VertexInfos_h
#define VertexInfos_h

#include "Platform.h"
#include "VertexDeclaration.h"
#include "VertexBufferBinding.h"

#include <atomic>

namespace Atl
{
    //! @brief Defines a vertex data set.
    class EXPORTED VertexInfos
    {
        //! @brief The declaration for this vertex set.
        VertexDeclarationPtr mDeclaration;
        
        //! @brief The buffer bindings for this vertex set.
        VertexBufferBindingPtr mBinding;
        
        //! @brief Index of the first vertex in the data set.
        std::atomic < std::size_t > mBaseVertex;
        
        //! @brief Number of vertexes in the data set.
        std::atomic < std::size_t > mCountVertexes;
        
    public:
        ATL_SHAREABLE(VertexInfos)
        
        //! @brief Constructs an empty data set.
        //! The VertexDeclaration and VertexBufferBinding structures are created for you, so
        //! you can directly use \ref declaration() and \ref binding().
        VertexInfos();
        
        //! @brief Constructs a new VertexInfos.
        //! @param declaration A pointer to a VertexDeclaration for this data set.
        //! @param binding A pointer to a VertexBufferBinding for this data set.
        //! @param base The index of the first vertex for this data set.
        //! @param count The number of vertexes in this data set.
        VertexInfos(const VertexDeclarationPtr& declaration, const VertexBufferBindingPtr& binding, std::size_t base, std::size_t count);
        
        //! @brief Returns the declaration.
        VertexDeclarationPtr declaration() const;
        
        //! @brief Returns the binding.
        VertexBufferBindingPtr binding() const;
        
        //! @brief Returns the base vertex.
        std::size_t baseVertex() const;
        
        //! @brief Changes the base vertex in the data set.
        void setBaseVertex(std::size_t base);
        
        //! @brief Returns the number of vertexes.
        std::size_t vertexesCount() const;
        
        //! @brief Changes the number of vertexes in this data set.
        void setVertexesCount(std::size_t count);
        
        //! @brief Returns the number of bytes for the start of the data set in the given
        //! buffer source.
        std::size_t baseOffset(unsigned short source) const;
        
        //! @brief Adds an element to the VertexDeclaration in this data set.
        //! This function is a shortcut for this->declaration()->addElement().
        //! @param source The binding from which we should take the HardwareBuffer for the
        //! created element.
        //! @param offset The offset where the element's list actually starts in the bound
        //! buffer.
        //! @param type The type that describe this element.
        //! @param meaning The element's meaning.
        //! @return A reference to the constructed VertexElement.
        const VertexElement& addElement(unsigned short source, std::size_t offset, VertexElementType type, const std::string& meaning);

        //! @brief Sets the \ref VertexDeclaration for this data set.
        void setDeclaration(const VertexDeclarationPtr& declaration);
    };
    
    typedef std::shared_ptr < VertexInfos > VertexInfosPtr;
}

#endif /* VertexInfos_h */
