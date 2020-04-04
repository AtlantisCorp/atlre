//
//  TransformationRenderCache.h
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#ifndef TransformationRenderCache_h
#define TransformationRenderCache_h

#include "RenderCache.h"
#include "ShaderVariableCommand.h"

namespace Atl
{
    class Transformation;

    //! @brief RenderCache for a \ref Transformation.
    //! The only purpose of this cache is to create a ShaderVariableCommand for the given
    //! renderer and manage it when the Transformation is modified. This is done 
    //! in \ref Transformation::build().
    class EXPORTED TransformationRenderCache : public RenderCache < Transformation >
    {
        //! @brief The command we generated for the renderer.
        ShaderVariableCommandPtr mCommand;

    public:
        ATL_SHAREABLE(TransformationRenderCache)

        //! @brief Constructs a new RenderCache.
        //! mCommand is created in this function thanks to the renderer, and stays available
        //! untill destruction of this object.
        TransformationRenderCache(Renderer& renderer, Transformation& owner);

        //! @brief Constructs a new RenderCache.
        //! The RenderCommand is shared with another cache (or any object that holds the cache). The
        //! passed pointer must not be null or this function throws a \ref NullError.
        TransformationRenderCache(Renderer& renderer, Transformation& owner, const ShaderVariableCommandPtr& command);

        //! @brief Returns a pointer to the command in this RenderCache.
        ShaderVariableCommandPtr command() const;

        //! @brief Changes the command for this RenderCache.
        void setCommand(const ShaderVariableCommandPtr& command);

        //! @brief Does nothing.
        std::future < void > build(Renderer&);

        //! @brief Adds the command into the RenderCommand.
        std::future < void > render(RenderCommand& command) const;

        //! @brief Returns zero: no size is applicable.
        std::size_t size(Renderer&) const;
    };

    typedef std::shared_ptr < TransformationRenderCache > TransformationRenderCachePtr;
}

#endif // TransformationRenderCache_h