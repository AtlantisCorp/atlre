//
//  ModelRenderNode.cpp
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#include "ModelRenderNode.h"

namespace Atl
{
    ModelRenderNode::ModelRenderNode(const Node::Shared& node,
        const ModelPtr& model,
        const std::size_t& maxChildren,
        const std::size_t& maxRenderables)
    : RenderNode(node, maxChildren, maxRenderables), mModel(model)
    {
        addRenderable(model);
    }

    ModelPtr ModelRenderNode::model() const
    {
        return std::atomic_load(&mModel);
    }

    void ModelRenderNode::setModel(const ModelPtr& model)
    {
        std::atomic_store(&mModel, model);
        send(&Listener::onModelModified, *this, (const Model&)*model);
    }
}