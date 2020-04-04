//
//  RenderNode.cpp
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#include "RenderNode.h"
#include "RenderCommand.h"
#include "Renderer.h"

namespace Atl
{
    RenderNode::RenderNode(const Node::Shared& parent, 
        const std::size_t& maxChildren,
        const std::size_t& maxRenderables)
    : Node(parent, maxChildren), mMaxRenderables(maxRenderables), mRenderRenderablesFirst(false)
    {
        mTasks = std::make_shared < RenderTaskContainer >();
        if (!mTasks) 
            throw NullError("RenderNode", "RenderNode", "Cannot allocate RenderTaskContainer.");
    }

    std::future < void > RenderNode::build(Renderer& renderer)
    {
        return std::async(std::launch::async, [this, &renderer]()
        {
            // Sends our WillBuild event right now, and wait for completion.

            send(&Listener::onRenderNodeWillBuild, *this, renderer);

            {
                std::lock_guard l(mMutex);
                mTasks->clear();

                for (const RenderablePtr& renderable : mRenderables)
                {
                    mTasks->add(renderable);
                }

                // Finally, clean our node.

                Node::clean();
            }

            // Sends our DidBuild event here.

            send(&Listener::onRenderNodeDidBuild, *this, renderer);
        });
    }

    std::future < void > RenderNode::render(RenderCommand& cmd) const
    {
        return std::async(std::launch::async, [this, &cmd]()
        {
            // If mOwnRenderCommand is true, we have to render everything into our own sub command
            // for the RenderCommand not to be impacted. This is the case by default, and derived
            // classes (like MaterialRenderNode) renders directly in the passed command (but are
            // ordered nodes).

            if (mOwnRenderCommand && (!mOwnCommand || &(mOwnCommand->renderer()) != &(cmd.renderer())))
            {
                const_cast < RenderNode& >(*this).mOwnCommand = 
                    cmd.renderer().newCommand < RenderCommand >();

                if (!mOwnCommand)
                    throw NullError("RenderNode", "render", "Null RenderCommand created.");
            }

            if (Node::isTouched())
                const_cast < RenderNode& >(*this).build(cmd.renderer()).get();

            if (!mOwnRenderCommand)
                mTasks->render(cmd).get();
            else
                mTasks->render(*mOwnCommand).get();
        });
    }
    
    std::size_t RenderNode::size(Renderer& rhs) const
    {
        std::size_t total = 0;
        std::scoped_lock l(mMutex);
        
        for (const RenderablePtr& renderable : mRenderables)
        {
            if (!renderable)
                continue;
            
            total += renderable->size(rhs);
        }
        
        return total;
    }

    bool RenderNode::isUnorderedRender() const
    {
        return false;
    }

    void RenderNode::addRenderable(const RenderablePtr& rhs)
    {
        if (!rhs)
            throw NullError("RenderNode", "addRenderable", "Null Renderable.");

        {
            std::lock_guard l(mMutex);

            if (mMaxRenderables > 0 && mRenderables.size() == mMaxRenderables)
                throw RenderNodeMaxRenderables("RenderNode", "addRenderable", "Renderables limit of %i reached.", 
                                            mMaxRenderables.load());

            mRenderables.push_back(rhs);
            TimeTouchable::touch();
        }

        send(&Listener::onRenderNodeDidAddRenderable, *this, (const Renderable&)*rhs);
    }

    void RenderNode::addRenderables(const RenderableList& rhs)
    {
        if (rhs.size() > 0)
        {
            {
                std::lock_guard l(mMutex);

                if (mMaxRenderables > 0 && mRenderables.size() + rhs.size() > mMaxRenderables)
                    throw RenderNodeMaxRenderables("RenderNode", "addRenderables", "Renderables limit of %i reached.", 
                                                mMaxRenderables.load());

                for (const RenderablePtr& renderable : rhs)
                {
                    if (!renderable)
                        throw NullError("RenderNode", "addRenderables", "Null Renderable.");

                    mRenderables.push_back(renderable);
                }

                TimeTouchable::touch();
            }

            // Now send our events. NOTES: We cannot have any null Renderable 
            // in the list because if we had one, and exception would have been
            // thrown so just send the events.

            for (const RenderablePtr& renderable : rhs)
                send(&Listener::onRenderNodeDidAddRenderable, *this, (const Renderable&)*renderable);
        }
    }

    void RenderNode::insertRenderable(const std::size_t& idx, const RenderablePtr& rhs)
    {
        if (!rhs)
            throw NullError("RenderNode", "insertRenderable", "Null Renderable.");

        {
            std::lock_guard l(mMutex);

            if (mMaxRenderables > 0 && mRenderables.size() == mMaxRenderables)
                throw RenderNodeMaxRenderables("RenderNode", "addRenderable", "Renderables limit of %i reached.", 
                                            mMaxRenderables.load());

            if (idx >= mRenderables.size())
                throw OutOfRange("RenderNode", "insertRenderable", "Index %i is out of range.", idx);

            RenderableList::const_iterator it = mRenderables.begin();
            std::advance(it, idx);
            mRenderables.insert(it, rhs);

            TimeTouchable::touch();
        }

        send(&Listener::onRenderNodeDidAddRenderable, *this, (const Renderable&)*rhs);
    }

    void RenderNode::insertRenderables(const std::size_t& idx, const RenderableList& rhs)
    {
        if (rhs.empty())
            return;

        {
            std::lock_guard l(mMutex);

            if (mMaxRenderables > 0 && mRenderables.size() + rhs.size() > mMaxRenderables)
                throw RenderNodeMaxRenderables("RenderNode", "insertRenderables", "Renderables limit of %i reached.", 
                                            mMaxRenderables.load());
            
            if (idx >= mRenderables.size())
                throw OutOfRange("RenderNode", "insertRenderables", "Index %i is out of range.", idx);

            RenderableList::const_iterator it = mRenderables.begin();
            std::advance(it, idx);
            mRenderables.insert(it, rhs.begin(), rhs.end());

            TimeTouchable::touch();
        }

        for (const RenderablePtr& renderable : rhs)
            send(&Listener::onRenderNodeDidAddRenderable, *this, (const Renderable&)*renderable);
    }

    void RenderNode::removeRenderableAt(const std::size_t& idx)
    {
        RenderablePtr removed = nullptr;

        {
            std::lock_guard l(mMutex);

            if (idx >= mRenderables.size())
                throw OutOfRange("RenderNode", "removeRenderableAt", "Index %i is out of range.", idx);

            RenderableList::const_iterator it = mRenderables.begin();
            std::advance(it, idx);

            removed = *it;
            mRenderables.erase(it);

            TimeTouchable::touch();
        }

        send(&Listener::onRenderNodeDidRemoveRenderable, *this, (const Renderable&)*removed);
    }

    void RenderNode::removeRenderable(const RenderablePtr& rhs)
    {
        if (!rhs)
            return;

        bool removed = false;

        {
            std::lock_guard l(mMutex);

            RenderableList::const_iterator it = std::find(mRenderables.begin(), mRenderables.end(), rhs);

            if (it != mRenderables.end())
            {
                mRenderables.erase(it);
                removed = true;
            }
            

            TimeTouchable::touch();
        }

        if (removed)
            send(&Listener::onRenderNodeDidRemoveRenderable, *this, (const Renderable&)*rhs);
    }

    void RenderNode::removeAllRenderables()
    {
        RenderableList renderables;

        std::vector < std::future < void > > events;
        events.reserve(renderables.size());

        {
            std::lock_guard l(mMutex);

            renderables = mRenderables;
            mRenderables.clear();

            TimeTouchable::touch();
        }

        for (const RenderablePtr& rhs : renderables)
            events.push_back(send(&Listener::onRenderNodeDidRemoveRenderable, *this, (const Renderable&)*rhs));
    }

    std::size_t RenderNode::renderablesCount() const
    {
        std::lock_guard l(mMutex);
        return mRenderables.size();
    }

    Renderable& RenderNode::renderableAt(const std::size_t& idx)
    {
        std::lock_guard l(mMutex);

        if (idx >= mRenderables.size())
            throw OutOfRange("RenderNode", "renderableAt", "Index %i is out of range.", idx);

        return *mRenderables[idx];
    }

    const Renderable& RenderNode::renderableAt(const std::size_t& idx) const
    {
        std::lock_guard l(mMutex);

        if (idx >= mRenderables.size())
            throw OutOfRange("RenderNode", "renderableAt", "Index %i is out of range.", idx);

        return *mRenderables[idx];
    }

    std::size_t RenderNode::maxRenderables() const
    {
        return mMaxRenderables.load();
    }

    void RenderNode::setMaxRenderables(const std::size_t& num)
    {
        mMaxRenderables.store(num);
    }

    bool RenderNode::renderRenderablesFirst() const
    {
        return mRenderRenderablesFirst;
    }

    void RenderNode::setRenderRenderablesFirst(bool rhs)
    {
        mRenderRenderablesFirst = rhs;
        TimeTouchable::touch();
    }

    bool RenderNode::hasAABB() const
    {
        return false;
    }

    AABB RenderNode::aabb() const
    {
        std::lock_guard l(mMutex);
        return mAABB;
    }

    bool RenderNode::isVisible() const
    {
        return mIsVisible;
    }

    void RenderNode::setVisible(bool rhs)
    {
        mIsVisible = rhs;
    }

    std::future < void > RenderNode::render(RenderCommand& command, const Frustum& frustum) const
    {
        return std::async(std::launch::async, [this, &command, &frustum]()
        {
            // We render this node only if visible.

            if (!isVisible())
                return;

            // If mOwnRenderCommand is true, we have to render everything into our own sub command
            // for the RenderCommand not to be impacted. This is the case by default, and derived
            // classes (like MaterialRenderNode) renders directly in the passed command (but are
            // ordered nodes).

            if (mOwnRenderCommand && (!mOwnCommand || &(mOwnCommand->renderer()) != &(command.renderer())))
            {
                const_cast < RenderNode& >(*this).mOwnCommand = 
                    command.renderer().newCommand < RenderCommand >();

                if (!mOwnCommand)
                    throw NullError("RenderNode", "render", "Null RenderCommand created.");
            }

            if (mCullOnFrustum)
            {
                // Now we have to check if this node is visible for the Frustum. If we have no AABB, then
                // we consider this node as always visible for the Frustrum.

                bool isFrustumVisible = hasAABB();

                if (isFrustumVisible)
                    isFrustumVisible = frustum.isBoxVisible(mAABB.min, mAABB.max);

                if (!isFrustumVisible)
                    return;
            }

            // Now we know that we are visible. We have to render our renderables and our children, 
            // first depending on mRenderRenderablesFirst. 

            if (mRenderRenderablesFirst)
            {
                if (!mOwnRenderCommand)
                    render(command);
                else
                    render(*mOwnCommand);
            }

            if (mRenderChildren)
            {
                std::lock_guard l(Node::mMutex);

                for (const Node::Shared& node : Node::mChildren)
                {
                    Shared renderNode = std::dynamic_pointer_cast < RenderNode >(node);

                    if (!renderNode)
                        throw NullError("RenderNode", "render", "Null RenderNode cast.");

                    if (!mOwnRenderCommand)
                        renderNode->render(command, frustum);
                    else
                        renderNode->render(*mOwnCommand, frustum);
                }
            }

            if (!mRenderRenderablesFirst)
            {
                if (!mOwnRenderCommand)
                    render(command);
                else
                    render(*mOwnCommand);
            }
        });
    }

    void RenderNode::setOwnRenderCommand(bool rhs)
    {
        mOwnRenderCommand = rhs;
    }

    bool RenderNode::ownRenderCommand() const
    {
        return mOwnRenderCommand;
    }

    void RenderNode::setCullOnFrustum(bool rhs)
    {
        mCullOnFrustum = rhs;
    }

    bool RenderNode::cullOnFrustum() const
    {
        return mCullOnFrustum;
    }

    void RenderNode::setRenderChildren(bool rhs)
    {
        mRenderChildren = rhs;
    }

    bool RenderNode::isRenderChildren() const
    {
        return mRenderChildren;
    }

    bool RenderNode::isCulledFromFrustum(const Frustum& frustum) const
    {
        if (!isVisible())
            return true;

        if (!hasAABB())
            return false;

        std::lock_guard l(mMutex);
        return frustum.isBoxVisible(mAABB.min, mAABB.max);
    }
}
