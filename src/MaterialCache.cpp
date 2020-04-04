//
//  MaterialCache.cpp
//  atl
//
//  Created by jacques tronconi on 01/04/2020.
//

#include "MaterialCache.h"
#include "Material.h"

namespace Atl
{
    MaterialCache::MaterialCache(Renderer& rhs, Material& material)
    : RenderCache(rhs, material)
    {
        mCommands.resize(static_cast < int >(MaterialElement::Max), nullptr);
    }

    std::future < void > MaterialCache::build(Renderer& rhs)
    {
        return std::async(std::launch::async, [this, &rhs]()
        {
            send(&Listener::onRenderableWillBuild, (Renderable&)*this, rhs).get();

            {
                std::lock_guard l(mMutex);
                MaterialLockGuard ll(mOwner);
                Material::ElementMap& elements = mOwner.elements();

                for (auto& pair : elements)
                {
                    ShaderVariableCommandPtr& command = mCommands[static_cast < int >(pair.first)];

                    if (command)
                        command->setVariableValue(pair.second.value());

                    else
                    {
                        command = rhs.newCommand < ShaderVariableCommand >();
                        command->setShaderVariable(pair.second);
                    }
                }
            }

            send(&Listener::onRenderableDidBuild, (Renderable&)*this, rhs).get();
        });
    }

    std::future < void > MaterialCache::render(RenderCommand& cmd) const
    {
        return std::async(std::launch::async, [this, &cmd]()
        {
            send(&Listener::onRenderableWillRender, (const Renderable&)*this, (RenderCommand&)cmd).get();

            {
                std::lock_guard l(mMutex);
                cmd.addSubCommands(mCommands, true);
            }

            send(&Listener::onRenderableDidRender, (const Renderable&)*this, (RenderCommand&)cmd).get();
        });
    }

    std::size_t MaterialCache::size(Renderer&) const
    {
        std::lock_guard l(mMutex);
        std::size_t total = 0;

        for (auto& command : mCommands)
        {
            if (!command)
                continue;

            total += command->variableValueSize();
        }

        return total;
    }
}
