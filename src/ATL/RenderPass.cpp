//
//  RenderPass.cpp
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "RenderPass.h"

namespace Atl
{
    // --------------------------------------------------------------------------------------------
    // RenderPassManager

    RenderPassPtr RenderPassManager::findName(const std::string& name) const 
    {
        return find([&name](const RenderPassPtr& rhs){ return rhs->name() == name; });
    }

    // --------------------------------------------------------------------------------------------
    // RenderPass

    RenderPass::RenderPass(Renderer& renderer, const std::string& name, const RenderPipelinePtr& pipeline, const RenderCommandPtr& command)
    : RenderObject(renderer), Named(name)
    , mPipeline(pipeline)
    , mCommand(command)
    {

    }
}