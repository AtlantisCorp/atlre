//
//  RenderPass.h
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ATL_RENDERPASS_H
#define ATL_RENDERPASS_H

#include "Platform.h"
#include "Named.h"
#include "RenderPipeline.h"
#include "RenderCommand.h"

namespace Atl
{
    class RenderPass;
    class RenderPassManager;

    ATL_BASE_RESOURCE(RenderPass)

    //! @brief Manages multiple RenderPass.
    struct RenderPassManager : public ShareableManager < RenderPass >
    {
        //! @brief Finds a pass by its name.
        RenderPassPtr findName(const std::string& name) const;
    };

    //! @brief An object used to render a RenderCommand with a RenderPipeline.
    //! This is the main Pass for our Renderer: one call to RenderPass::render() actually
    //! draws a whole scene into a RenderTarget.
    class RenderPass : public RenderObject,
                       public Named
    {
        //! @brief Atomic pointer to the RenderPipeline.
        RenderPipelinePtr mPipeline;

        //! @brief Atomic pointer to the RenderCommand to draw.
        RenderCommandPtr mCommand;

    public:
        ATL_SHAREABLE(RenderPass)

        //! @brief Constructs a new RenderPass.
        RenderPass(Renderer& renderer, const std::string& name, const RenderPipelinePtr& pipeline, const RenderCommandPtr& command);

        //! @brief Destructor.
        virtual ~RenderPass() = default;

        //! @brief Returns \ref mPipeline.
        virtual inline RenderPipelinePtr pipeline() const { 
            return std::atomic_load(&mPipeline); 
        } 

        //! @brief Sets \ref mPipeline.
        virtual inline void setPipeline(const RenderPipelinePtr& pipeline) {
            std::atomic_store(&mPipeline, pipeline);
        }

        //! @brief Returns \ref mCommand.
        virtual inline RenderCommandPtr command() const {
            return std::atomic_load(&mCommand);
        }

        //! @brief Sets \ref mCommand.
        virtual inline void setCommand(const RenderCommandPtr& command) {
            std::atomic_store(&mCommand, command);
        }
    };
}

#endif // ATL_RENDERPASS_H