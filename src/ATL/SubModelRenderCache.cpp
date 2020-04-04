//
//  SubModelRenderCache.cpp
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#include "SubModelRenderCache.h"
#include "SubModel.h"

namespace Atl
{
    SubModelRenderCache::SubModelRenderCache(Renderer& renderer, SubModel& subModel)
    : RenderCache(renderer, subModel)
    {
        mIndexData = nullptr;
        mInfos = nullptr;
        mDrawVertexes = nullptr;
        mDrawIndexed = nullptr;
    }

    std::future < void > SubModelRenderCache::build(Renderer& rhs)
    {
        return std::async(std::launch::async, [this, &rhs]()
        {
            if (!isFrom(rhs))
                throw RenderableInvalidRenderer("SubModelRenderCache", "build", "Invalid Renderer.");

            send(&Listener::onRenderableWillBuild, (Renderable&)*this, rhs).get();

            // First, takes the Indexed data set.

            if (mOwner.hasIndexes())
            {
                const IndexBufferData& indexes = mOwner.indexes();

                HardwareBufferPtr memBuffer = indexes.buffer();
                MemBufferPtr asMemBuffer = std::dynamic_pointer_cast < MemBuffer >(memBuffer);

                if (asMemBuffer)
                {
                    // Creates the index data only if it has not been yet.

                    if (!mIndexData || !(mIndexData->buffer()))
                    {
                        RenderHdwBufferPtr hdwBuffer = rhs.hdwBufferManager().findOrCreateRelated(asMemBuffer);

                        if (!hdwBuffer)
                            throw NullError("SubModelRenderCache", "build", "Null RenderHdwBuffer for MemBuffer %i.", asMemBuffer->index());
                        
                        mIndexData = IndexBufferData::New(indexes.elementsCount(), hdwBuffer, indexes.type());
                    }

                    // Else just update the buffer and infos.

                    else  
                    {
                        mIndexData->buffer()->copy(*memBuffer);
                        mIndexData->setElementsCount(indexes.elementsCount());
                        mIndexData->setType(indexes.type());
                    }
                }

                else  
                {
                    RenderHdwBufferPtr hdwBuffer = std::dynamic_pointer_cast < RenderHdwBuffer >(memBuffer);

                    if (!hdwBuffer)
                        throw NullError("SubModelRenderCache", "build", "HardwareBuffer is not supported.");

                    if (!(&hdwBuffer->renderer() == &rhs))
                        throw RenderableInvalidRenderer("SubModelRenderCache", "build", "HardwareBuffer is from another Renderer.");

                    mIndexData->setBuffer(hdwBuffer);
                    mIndexData->setElementsCount(indexes.elementsCount());
                    mIndexData->setType(indexes.type());
                }
            }

            // Then, creates the VertexInfos.

            mInfos = VertexInfos::New(
                mOwner.vertexInfos().declaration(), 
                VertexBufferBinding::New(),
                mOwner.vertexInfos().baseVertex(),
                mOwner.vertexInfos().vertexesCount());

            VertexBufferBindingPtr memBuffers = mOwner.vertexInfos().binding();
            VertexBufferBindingPtr hdwBuffers = mInfos->binding();

            for (auto const& pair : memBuffers->bindings())
            {
                // For each Buffer Binding, we try to find the corresponding index. Each
                // MemBuffer is associated to a unique index.

                MemBufferPtr memBuffer = std::dynamic_pointer_cast < MemBuffer >(pair.second);

                if (memBuffer)
                {
                    RenderHdwBufferPtr hdwBuffer = rhs.hdwBufferManager().findOrCreateRelated(memBuffer);

                    if (!hdwBuffer) 
                        throw NullError("SubModelRenderCache", "build", "RenderHdwBuffer for MemBuffer %i is null.", static_cast < unsigned >(memBuffer->index()));
                    
                    hdwBuffers->set(pair.first, hdwBuffer);
                }

                else
                {
                    RenderHdwBufferPtr hdwBuffer = std::dynamic_pointer_cast < RenderHdwBuffer >(pair.second);

                    if (!hdwBuffer)
                        throw NullError("SubModelRenderCache", "build", "Cannot find type of HardwareBuffer.");

                    if (!(&hdwBuffer->renderer() == &rhs))
                    {
                        RenderHdwBufferPtr newHdwBuffer = rhs.hdwBufferManager().copy(hdwBuffer);

                        if (!newHdwBuffer)
                            throw NullError("SubModelRenderCache", "build", "Cannot copy HardwareBuffer %i.", hdwBuffer->index());
                        
                        hdwBuffer = newHdwBuffer;
                    }

                    hdwBuffers->set(pair.first, hdwBuffer);
                }
            }

            // Creates the RenderCommands and update them.

            if (mIndexData)
            {
                if (!mDrawIndexed)
                {
                    mDrawVertexes = nullptr;
                    mDrawIndexed = rhs.newCommand < DrawIndexedArraysCommand >();
                }
                mDrawIndexed->construct(mInfos, mIndexData);
            }

            else 
            {
                if (!mDrawVertexes)
                {
                    mDrawVertexes = rhs.newCommand < DrawVertexArraysCommand >();
                    mDrawIndexed = nullptr;
                }
                mDrawVertexes->construct(mInfos);
            }

            send(&Listener::onRenderableDidBuild, (Renderable&)*this, rhs).get();
        });
    }

    std::future < void > SubModelRenderCache::render(RenderCommand& cmd) const
    {
        return std::async(std::launch::async, [this, &cmd]()
        {
            send(&Listener::onRenderableWillRender, (const Renderable&)*this, cmd).get();

            if (mDrawIndexed)
                cmd.addSubCommand(mDrawIndexed);
            else if (mDrawVertexes)
                cmd.addSubCommand(mDrawVertexes);

            send(&Listener::onRenderableDidRender, (const Renderable&)*this, cmd).get();
        });
    }

    std::size_t SubModelRenderCache::size(Renderer&) const
    {
        std::size_t total = 0;

        if (mInfos && mInfos->binding())
        {
            for (auto const& pair : mInfos->binding()->bindings())
            {
                if (pair.second)
                    total += pair.second->size();
            }
        }

        if (mIndexData && mIndexData->buffer())
            total += mIndexData->buffer()->size();

        return total;
    }
}