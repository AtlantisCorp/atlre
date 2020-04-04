//
//  Node.cpp
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#include "Node.h"

namespace Atl
{
    Node::Node(const Shared& parent, const std::size_t& maxChildren)
    : mParent(parent), mMaxChildren(maxChildren)
    {

    }

    Node::~Node() 
    {
        send(&NodeListener::onNodeWillDestroy, *this);
    }

    void Node::addChild(const Shared& child)
    {
        if (!child)
            throw NullError("Node", "addChild", "Null child passed to this Node.");

        {
            std::lock_guard l(mMutex);

            if (mMaxChildren > 0 && mChildren.size() == mMaxChildren)
                throw NodeMaxChildren("Node", "addChild", "Maximum number of children (%i) reached.", mMaxChildren.load());

            mChildren.push_back(child);
            child->mParent = shared_from_this();
            
            TimeTouchable::touch();
        }

        child->send(&NodeListener::onNodeParentDidChange, *child);
        send(&NodeListener::onNodeDidAddChild, *this, child);
    }

    Node& Node::childAt(unsigned int idx)
    {
        std::lock_guard l(mMutex);

        if (idx >= mChildren.size())
            throw OutOfRange("Node", "childAt", "Index %i is out of children range.", idx);

        return *mChildren[idx];
    }

    const Node& Node::childAt(unsigned int idx) const
    {
        std::lock_guard l(mMutex);

        if (idx >= mChildren.size())
            throw OutOfRange("Node", "childAt", "Index %i is out of children range.", idx);

        return *mChildren[idx];
    }

    std::size_t Node::childrenCount() const
    {
        std::lock_guard l(mMutex);
        return mChildren.size();
    }

    void Node::removeChild(const Shared& child)
    {
        if (!child)
            throw NullError("Node", "removeChild", "Null child.");

        {
            std::lock_guard l(mMutex);

            auto it = std::find(mChildren.begin(), mChildren.end(), child);

            if (it != mChildren.end())
            {
                mChildren.erase(it);
                child->mParent.reset();
                TimeTouchable::touch();
            }
        }

        child->send(&NodeListener::onNodeParentDidChange, *child);
        send(&NodeListener::onNodeDidRemoveChild, *this, child);
    }

    void Node::removeChildAt(unsigned int idx)
    {
        Shared node = nullptr;

        {
            std::lock_guard l(mMutex);

            if (idx >= mChildren.size())
                throw OutOfRange("Node", "removeChildAt", "Index %i is out of range.", idx);

            node = mChildren[idx];
            mChildren.erase(mChildren.begin() + idx);

            TimeTouchable::touch();
            node->mParent.reset();
        }

        if (node)
        {
            node->send(&NodeListener::onNodeParentDidChange, *node);
            send(&NodeListener::onNodeDidRemoveChild, *this, (const Shared&)node);
        }
    }

    void Node::removeAllChildren()
    {
        ChildList children;

        {
            std::lock_guard l(mMutex);

            children = mChildren;
            mChildren.clear();

            TimeTouchable::touch();
        }

        for (Shared& node : children)
        {
            node->mParent.reset();
            node->send(&NodeListener::onNodeParentDidChange, *node);
            send(&NodeListener::onNodeDidRemoveChild, *this, (const Shared&)node);
        }
    }

    std::size_t Node::maxChildren() const
    {
        return mMaxChildren.load();
    }

    void Node::setMaxChildren(const std::size_t& num)
    {
        mMaxChildren.store(num);
    }

    void Node::clean() const
    {
        TimeTouchable::clean();
        std::lock_guard l(mMutex);

        for (const Shared& node : mChildren)
            node->clean();
    }

    bool Node::isTouched() const
    {
        if (TimeTouchable::isTouched())
            return true;

        std::lock_guard l(mMutex);

        for (const Shared& node : mChildren)
        {
            if (node->isTouched())
                return true;
        }
    }

    Node::Shared Node::parent() const
    {
        return mParent.lock();
    }
}