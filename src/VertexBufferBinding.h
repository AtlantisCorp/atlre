//
//  VertexBufferBinding.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef VertexBufferBinding_h
#define VertexBufferBinding_h

#include "Platform.h"
#include "HardwareBuffer.h"

#include <map>

namespace Atl
{
    //! @brief Records the state of all the vertex buffer bindings required to
    //! provide a vertex declaration with the input data it needs for the vertex
    //! elements.
    class EXPORTED VertexBufferBinding
    {
    public:
        typedef std::map < unsigned short, HardwareBufferPtr > BufferMap;
        
    protected:
        //! @brief Maps of a source associated to an HardwareBuffer.
        BufferMap mBuffers;
        
    public:
        ATL_SHAREABLE(VertexBufferBinding)
        
        //! @brief Default constructor.
        VertexBufferBinding() = default;
        
        //! @brief Sets the buffer for a source.
        //! @param source The source to set.
        //! @param buffer The buffer to associate.
        void set(unsigned short source, const HardwareBufferPtr& buffer);
        
        //! @brief Unsets the buffer for a source.
        //! @param source The source to remove.
        void unset(unsigned short source);
        
        //! @brief Clears all bindings.
        void clear();
        
        //! @brief Returns all the bindings.
        const BufferMap& bindings() const;
        
        //! @brief Gets the buffer bound to the given source.
        const HardwareBufferPtr& bufferAt(unsigned short source) const;
        
        //! @brief Returns true if a buffer is bound in a source.
        bool isBufferBound(unsigned short source) const;
        
        //! @brief Returns the number of buffers in the map.
        std::size_t buffersCount() const;
        
        //! @brief Returns the next source index in the map.
        unsigned short nextSourceIndex() const;
        
        //! @brief Returns true if those bindings have a 'gap'.
        bool hasGaps() const;
    };
    
    typedef std::shared_ptr < VertexBufferBinding > VertexBufferBindingPtr;
}

#endif /* VertexBufferBinding_h */
