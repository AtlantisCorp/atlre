//
//  MovableRenderNode.cpp
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#include "MovableRenderNode.h"

namespace Atl
{
    MovableRenderNode::MovableRenderNode(const Node::Shared& parent, 
        const TransformationPtr& transformation, 
        const std::size_t& maxChildren,
        const std::size_t& maxRenderables)
    : RenderNode(parent, maxChildren, maxRenderables), mTransformation(transformation)
    {
        if (!mTransformation)
            throw NullError("MovableRenderNode", "MovableRenderNode", "Null Transformation.");

        addRenderable(mTransformation);
    }

    MovableRenderNode& MovableRenderNode::lookAt(const rvec3& target, const rvec3& up)
    {
        Transformation& trans = *std::atomic_load(&mTransformation);
        trans = Transformation::LookAt(trans.name(), trans.translation(), target, up);
        return *this;
    }

    Transformation& MovableRenderNode::transformation()
    {
        return *std::atomic_load(&mTransformation);
    }

    const Transformation& MovableRenderNode::transformation() const
    {
        return *std::atomic_load(&mTransformation);
    }

    rvec3 MovableRenderNode::position() const
    {
        const Transformation& tran = transformation();
        return tran.translation();
    }

    void MovableRenderNode::setTransformation(const TransformationPtr& rhs)
    {
        std::atomic_store(&mTransformation, rhs);
    }

    void MovableRenderNode::setPosition(const rvec3& rhs)
    {
        TransformationPtr tran = std::atomic_load(&mTransformation);
        tran->translate(rhs - tran->translation());
    }

    void MovableRenderNode::translate(const rvec3& rhs)
    {
        TransformationPtr tran = std::atomic_load(&mTransformation);
        tran->translate(rhs);
    }
}