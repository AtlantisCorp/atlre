//
//  SubModelRenderCache.h
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#ifndef ATL_SUBMODELRENDERCACHE_H
#define ATL_SUBMODELRENDERCACHE_H

#include "Renderable.h"
#include "RenderCache.h"
#include "DrawIndexedArraysCommand.h"
#include "DrawVertexArraysCommand.h"

namespace Atl
{
    class SubModel;

    //! @brief Defines the SubModelRenderCache Listener.
    struct SubModelRenderCacheListener : virtual public RenderableListener
    {
        //! @brief Destructor.
        virtual ~SubModelRenderCacheListener() = default;
    };

    //! @brief Defines a \ref RenderCache for a \ref SubModel.
    //! 
    //! The SubModel RenderCache creates: 
    //! - A \ref HardwareBuffer for each \ref MemBuffer in the VertexInfos's buffer binding.
    //! - A \ref HardwareBuffer for the indexed buffer if the SubModel has indexed data.
    //! - A copy of the \ref VertexDescriptor in the Vertex Infos.
    //! - A copy of the \ref VertexBufferBinding with the created buffers.
    //! - A copy of the \ref IndexBufferData if the SubModel has indexed data.
    //! - A \ref DrawVertexArraysCommand if the SubModel has no indexed data, for the copied
    //! VertexInfos set.
    //! - A \ref DrawIndexedArraysCommand if the SubModel has indexed data.
    //!
    //! When rendering, the RenderCache only renders its RenderCommand, either DrawVertexArraysCommand
    //! or DrawIndexedArraysCommand.
    class SubModelRenderCache : public RenderCache < SubModel >
    {
        //! @brief The IndexBufferData for this cache.
        IndexBufferDataPtr mIndexData;

        //! @brief The VertexInfos for this cache. The \ref VertexDescriptor is shared with the
        //! parent \ref SubModel's descriptor. However the \ref VertexBufferBinding is local to 
        //! this render cache.
        VertexInfosPtr mInfos;

        //! @brief The DrawVertexArraysCommand for this cache, if the \ref SubModel has no indexed
        //! data. Notes it is more efficient to draw indexed data.
        DrawVertexArraysCommandPtr mDrawVertexes;

        //! @brief The \ref DrawIndexedArraysCommand for this cache, if the \ref SubModel has indexed
        //! data ready.
        DrawIndexedArraysCommandPtr mDrawIndexed;

    public:
        ATL_SHAREABLE(SubModelRenderCache)

        //! @brief Defines the listener.
        typedef SubModelRenderCacheListener Listener;

        //! @brief Constructs a new SubModelRenderCache.
        SubModelRenderCache(Renderer& renderer, SubModel& subModel);

        //! @brief Builds the RenderCache.
        //! Throws a RenderableInvalidRenderer Error if the Renderer provided is not the one registered
        //! for this cache. The SubModel is locked with SubModelLockGuard.
        virtual std::future < void > build(Renderer& rhs);

        //! @brief Renders the \ref RenderCommandBase for the \ref SubModel.
        virtual std::future < void > render(RenderCommand& command) const;

        //! @brief Returns the size of all buffers in this cache.
        virtual std::size_t size(Renderer&) const;
    };

    //! @brief Defines a Pointer to the \ref SubModelRenderCache.
    typedef std::shared_ptr < SubModelRenderCache > SubModelRenderCachePtr;
}

#endif // ATL_SUBMODELRENDERCACHE_H