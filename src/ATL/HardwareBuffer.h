//
//  HardwareBuffer.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef HardwareBuffer_h
#define HardwareBuffer_h

#include "Platform.h"
#include "Touchable.h"
#include "MakeUniqueIndex.h"

#include <vector>
#include <mutex>

namespace Atl
{
    //! @brief Enumerates some kind of informations about what can contain
    //! the HardwareBuffer.
    enum class HardwareBufferType
    {
        //! @brief Should contain Vertexes data set.
        Vertex,

        //! @brief Should contain Indexes data set.
        Index,

        //! @brief Other containers.
        Misc
    };

    //! @brief Shortcut for HardwareBufferType.
    using HBT = HardwareBufferType;

    //! @brief A base class for every buffer that may hold renderable data.
    class EXPORTED HardwareBuffer : public TimeTouchable
    {
        //! @brief The type used to initialize this HardwareBuffer.
        std::atomic < HBT > mType;

    public:
        typedef std::shared_ptr < HardwareBuffer > pointer_type;

        //! @brief Constructs a HardwareBuffer.
        //! @param type The type for this buffer. Default value to HBT::Vertex, 
        //! but you should set it to what you need.
        HardwareBuffer(const HBT& type = HBT::Vertex);
        
        //! @brief Default destructor.
        virtual ~HardwareBuffer() = default;
        
        //! @brief Returns true if this buffer is in memory.
        virtual bool isMemBuffer() const = 0;
        
        //! @brief Returns the size of this buffer.
        virtual std::size_t size() const = 0;
        
        //! @brief Allocates (or reallocates) some memory for this buffer.
        //! @note You should use \ref lock() before using this memory.
        //! @param sz The size, in bytes, of the future buffer.
        //! @param ptr If non null, the new data in this buffer.
        virtual void allocate(const std::size_t& sz, const void* ptr = nullptr) = 0;
        
        //! @brief Locks the buffer for reading or writing of the internal memory.
        virtual void lock() const = 0;
        
        //! @brief Unlocks the buffer after an i/o operation.
        virtual void unlock() const = 0;
        
        //! @brief Returns a pointer to the internal data.
        //! @note You should use \ref lock() before using this function.
        virtual void* data() = 0;
        
        //! @brief Returns a pointer to the internal data.
        //! @note You should use \ref lock() before using this function.
        virtual const void* data() const = 0;
        
        //! @brief Invalidates the internal data pointer.
        //! @note You should use \ref unlock() after this function, if you locked
        //! it previously.
        virtual void undata() const = 0;
        
        //! @brief Creates a new HardwareBuffer as a copy from this one.
        virtual pointer_type copy() const = 0;

        //! @brief Returns the index of this buffer, if one, or zero.
        virtual std::uint64_t index() const = 0;

        //! @brief Returns the type for this HardwareBuffer.
        virtual HBT type() const;

        //! @brief Sets \ref mType.
        virtual void setType(const HBT& type);

        //! @brief Copies another HardwareBuffer.
        //! Both HardwareBuffers are locked during the process. \ref allocate(), \ref size, 
        //! \ref data() and \ref undata() are used to copy given buffer.
        virtual void copy(const HardwareBuffer& rhs);
    };
    
    typedef HardwareBuffer::pointer_type HardwareBufferPtr;
    typedef std::lock_guard < HardwareBuffer > HardwareBufferLockGuard;
    typedef std::lock_guard < const HardwareBuffer > HardwareBufferLockGuardCst;
    
    //! @brief A Buffer in the CPU memory (and not on the GPU!).
    class EXPORTED MemBuffer : public HardwareBuffer, 
                               public MakeUniqueIndex < MemBuffer >
    {
        //! @brief Our actual memory.
        std::vector < char > mBuffer;
        
        //! @brief A mutex.
        mutable std::mutex mMutex;

    public:
        
        //! @brief Default constructor.
        MemBuffer(const HBT& type = HBT::Vertex);
        
        //! @brief Constructs a MemBuffer with actual data.
        MemBuffer(const std::vector < char >& rhs);
        
        //! @brief Constructs a MemBuffer with C-like data.
        MemBuffer(const char* buffer, std::size_t sz);
        
        //! @brief Default copy constructor.
        MemBuffer(const MemBuffer&);
        
        //! @brief Returns always true.
        bool isMemBuffer() const;
        
        //! @brief Returns the buffer's size, in bytes.
        std::size_t size() const;
        
        //! @brief Allocates (or reallocates) some memory for this buffer.
        //! @note You should use \ref lock() before using this memory.
        //! @param sz The size, in bytes, of the future buffer.
        //! @param ptr If non null, the new data in this buffer.
        void allocate(const std::size_t& sz, const void* ptr = nullptr);
        
        //! @brief Locks the buffer for reading or writing of the internal memory.
        void lock() const;
        
        //! @brief Unlocks the buffer after an i/o operation.
        void unlock() const;
        
        //! @brief Returns a pointer to the internal data.
        //! @note You should use \ref lock() before using this function.
        void* data();
        
        //! @brief Returns a pointer to the internal data.
        //! @note You should use \ref lock() before using this function.
        const void* data() const;
        
        //! @brief Invalidates the internal data pointer.
        //! @note You should use \ref unlock() after this function, if you locked
        //! it previously.
        void undata() const;
        
        //! @brief Creates a new HardwareBuffer as a copy from this one.
        pointer_type copy() const;

        //! @brief Returns the index of this buffer, if one, or zero.
        std::uint64_t index() const;
    };

    //! @brief Pointer to a MemBuffer.
    typedef std::shared_ptr < MemBuffer > MemBufferPtr;
}

#endif /* HardwareBuffer_h */
