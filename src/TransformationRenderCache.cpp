//
//  TransformationRenderCache.cpp
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#include "TransformationRenderCache.h"
#include "Renderer.h"

namespace Atl
{
    TransformationRenderCache::TransformationRenderCache(Renderer& renderer, Transformation& owner)
    : RenderCache(renderer, owner)
    {
        mCommand = renderer.newCommand < ShaderVariableCommand >();

        if (!mCommand)
            throw NullError("TransformationRenderCache", "TransformationRenderCache", 
                            "Renderer failed to make new ShaderVariableCommand.");
    }

    TransformationRenderCache::TransformationRenderCache(Renderer& renderer, Transformation& owner, const ShaderVariableCommandPtr& command)
    : RenderCache(renderer, owner), mCommand(command)
    {
        if (!mCommand)
            throw NullError("TransformationRenderCache", "TransformationRenderCache", 
                            "Null ShaderVariableCommand passed.");
    }

    ShaderVariableCommandPtr TransformationRenderCache::command() const
    {
        return std::atomic_load(&mCommand);
    }

    void TransformationRenderCache::setCommand(const ShaderVariableCommandPtr& command)
    {
        std::atomic_store(&mCommand, command);
    }

    std::future < void > TransformationRenderCache::build(Renderer&)
    {
        return std::future < void >();
    }

    std::future < void > TransformationRenderCache::render(RenderCommand& command) const
    {
        return std::async(std::launch::async, [this, &command]()
        {
            ShaderVariableCommandPtr subCommand = std::atomic_load(&mCommand);

            if (!subCommand)
                throw NullError("TransformationRenderCache", "TransformationRenderCache", 
                                "Null ShaderVariableCommand passed.");

            command.addSubCommand(std::atomic_load(&subCommand));
        });
    }

    std::size_t TransformationRenderCache::size(Renderer&) const
    {
        return 0;
    }
}