//
//  RenderHdwBufferManager.h
//  atl
//
//  Created by jacques tronconi on 23/03/2020.
//

#ifndef RenderHdwBufferManager_h
#define RenderHdwBufferManager_h

#include "RenderHdwBuffer.h"
#include "RenderObjectManager.h"
#include "MemoryPool.h"

namespace Atl
{
    //! @brief Launched when a HardwareBuffer Type is not supported.
    struct RenderHdwBufferTypeNotSupported : public Error
    { using Error::Error; };
    
    //! @brief A Manager for all RenderHdwBuffer.
    class RenderHdwBufferManager :
    public RenderObjectManager <
    RenderHdwBuffer,
    RenderHdwBufferFactory >,
    public MemoryPoolListener
    {
        //! @brief An observer allocated by this class to observer any size changes upon
        //! hardware render buffers.
        class ThisRenderHdwBufferObserver : public RenderHdwBufferObserver
        {
            //! @brief This manager.
            RenderHdwBufferManager& mManager;
            
        public:
            
            //! @brief Constructs a new observer.
            ThisRenderHdwBufferObserver(RenderHdwBufferManager& rhs);
            
            //! \see RenderHdwBufferObserver::isAvailable.
            bool isAvailable(std::size_t oldsz, std::size_t newsz) const;
            
            //! \see RenderHdwBufferObserver::change.
            void change(std::size_t oldsz, std::size_t newsz);
        };
        
        //! @brief The memory pool for this manager.
        MemoryPool mPool;
        
        //! @brief The observer we allocate for this manager.
        RenderHdwBufferObserverPtr mObserver;

        //! @brief Boolean true if low memory means we tries to auto free unused buffers. Default
        //! value is true.
        std::atomic < bool > mTriesFreeOnLow;
        
    public:
        
        //! @brief Constructs a new manager.
        //! @param rhs The renderer that creates this manager.
        //! @param maxSize The maximum size used with MemoryPool.
        RenderHdwBufferManager(Renderer& rhs, std::size_t maxSize = 0);
        
        //! @brief Constructs a RenderHdwBuffer from its type.
        RenderHdwBufferPtr make(const std::type_index& type);
        
        //! @brief Adds a RenderHdwBuffer to this manager.
        //! No assertion is done on the RenderHdwBuffer size. The only check for size
        //! is done from the Observer registered in \ref make(). A buffer that doesn't hold
        //! this manager observer will not have its memory managed by this manager's pool.
        void add(const RenderHdwBufferPtr& buffer);
        
        //! @brief Returns true if given size is available in this pool.
        bool isSizeAvailable(std::size_t sz) const;

        //! @brief Finds a \ref RenderHdwBuffer related to a \ref MemBuffer index. If none
        //! were found, returns nullptr.
        RenderHdwBufferPtr findRelated(const MemBuffer::Index& index) const;

        //! @brief Tries to find a \ref RenderHdwBuffer related to a \ref MemBuffer
        //! index. If not found, tries to create it from the \ref MemBuffer given.
        RenderHdwBufferPtr findOrCreateRelated(const MemBufferPtr& buffer);

        //! @brief Creates the copy of a \ref RenderHdwBuffer.
        RenderHdwBufferPtr copy(const RenderHdwBufferPtr& buffer);

        //! @brief Removes all RenderHdwBuffers that aren't used anywhere.
        void removeUnusedBuffers();

        //! @brief Launched when MemoryPool is low on memory.
        //! If called, and \ref mTriesFreeOnLow(), this manager tries to remove unused buffers
        //! with \ref removeUnusedBuffers().
        void onMemoryLow(MemoryPool&);
    };
}

#endif /* RenderHdwBufferManager_h */
