//
//  VertexBufferBinding.cpp
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#include "VertexBufferBinding.h"
#include "Error.h"

namespace Atl
{
    void VertexBufferBinding::set(unsigned short source, const HardwareBufferPtr &buffer)
    {
        mBuffers[source] = buffer;
    }
    
    void VertexBufferBinding::unset(unsigned short source)
    {
        mBuffers.erase(mBuffers.find(source));
    }
    
    void VertexBufferBinding::clear()
    {
        mBuffers.clear();
    }
    
    const VertexBufferBinding::BufferMap& VertexBufferBinding::bindings() const
    {
        return mBuffers;
    }
    
    const HardwareBufferPtr& VertexBufferBinding::bufferAt(unsigned short source) const
    {
        auto it = mBuffers.find(source);
        
        if (it == mBuffers.end())
            throw OutOfRange("VertexBufferBinding", "bufferAt", "source %i is not available.", source);
        
        return it->second;
    }
    
    bool VertexBufferBinding::isBufferBound(unsigned short source) const
    {
        return mBuffers.find(source) == mBuffers.end();
    }
    
    std::size_t VertexBufferBinding::buffersCount() const
    {
        return mBuffers.size();
    }
    
    unsigned short VertexBufferBinding::nextSourceIndex() const
    {
        return mBuffers.rbegin()->first + 1;
    }
    
    bool VertexBufferBinding::hasGaps() const
    {
        if (mBuffers.empty())
            return false;
        
        return nextSourceIndex() != mBuffers.size();
    }
}
