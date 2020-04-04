//
//  Node.h
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#ifndef Node_h
#define Node_h

#include "Error.h"
#include "Touchable.h"
#include "Emitter.h"

namespace Atl
{
    //! @brief Error launched when Maximum number of children is reached.
    struct EXPORTED NodeMaxChildren : public Error 
    { using Error::Error; };

    class Node;

    //! @brief A Generic Node Listener.
    class EXPORTED NodeListener : public ListenerBase
    {
    public:
        //! @brief Defines the Emitter for this listener.
        typedef Node Emitter;

        //! @brief Destructs the listener.
        virtual ~NodeListener() = default;
        //! @brief Handles a child has been added.
        virtual void onNodeDidAddChild(Emitter&, const std::shared_ptr < Emitter >&) {}
        //! @brief Handles a child has been removed.
        virtual void onNodeDidRemoveChild(Emitter&, const std::shared_ptr < Emitter >&) {}
        //! @brief Handles the node's parent has been changed.
        virtual void onNodeParentDidChange(Emitter&) {}
        //! @brief Handles the node's will be destroyed.
        virtual void onNodeWillDestroy(Emitter&) {}
    };

    //! @brief A Generic Node.
    class EXPORTED Node : public std::enable_shared_from_this < Node >,
        virtual public TimeTouchable,
        virtual public Emitter
    {
    public:
        //! @brief A Shared Pointer to this Node.
        typedef std::shared_ptr < Node > Shared;
        //! @brief A Weak Pointer to this Node.
        typedef std::weak_ptr < Node > Weak;

        //! @brief Defines a Node's Child.
        typedef Shared Child;
        //! @brief Defines a Node's Child list.
        typedef std::vector < Child > ChildList;

    protected:
        //! @brief The Parent of this Node. Null if this node has no
        //! Parent (if this Node is root, for example).
        Weak mParent;

        //! @brief The Node's Children.
        ChildList mChildren;

        //! @brief The mutex.
        mutable std::mutex mMutex;

        //! @brief The maximum number of children by Node. If zero, this means
        //! unlimited amount of children.
        std::atomic < std::size_t > mMaxChildren;

    public:

        //! @brief Constructs a new Node.
        //! @param parent The Node's Parent. May be a null pointer if this Node
        //! is root, or if this Node is not added to any parent yet.
        //! @param maxChildren The maximum number of children this Node can have.
        Node(const Shared& parent = nullptr, const std::size_t& maxChildren = 0);

        //! @brief Default destructor.
        virtual ~Node();

        //! @brief Adds a child to this Node.
        //! @param child A non null Node Pointer. By convention, this function sets
        //! the child's parent to this Node. You don't have to update the chlid's
        //! parent node.
        virtual void addChild(const Shared& child);

        //! @brief Finds a child at given index.
        //! @param idx The index to check at. An \ref OutOfRange exception is launched
        //! if idx is invalid. Please check \ref childrenCount() value before.
        virtual Node& childAt(unsigned int idx);

        //! @brief Finds a child at given index.
        //! @param idx The index to check at. An \ref OutOfRange exception is launched
        //! if idx is invalid. Please check \ref childrenCount() value before.
        virtual const Node& childAt(unsigned int idx) const;

        //! @brief Returns the number of children in this Node.
        virtual std::size_t childrenCount() const;

        //! @brief Removes the given Node.
        virtual void removeChild(const Shared& child);

        //! @brief Removes the Node at given index.
        virtual void removeChildAt(unsigned int idx);

        //! @brief Removes all children from this Node.
        virtual void removeAllChildren();

        //! @brief Returns the maximum number of children in this node.
        virtual std::size_t maxChildren() const;

        //! @brief Changes the maximum number of children in this node.
        //! @note Depending on the derived class, a maximum number may be imposed
        //! and thus, calling this function may throw an Error if the derived class
        //! doesn't allow a change.
        virtual void setMaxChildren(const std::size_t& num);

        //! @brief Cleans this Node and all of its children.
        virtual void clean() const;

        //! @brief Returns true if this Node or one of its children is touched.
        virtual bool isTouched() const;

        //! @brief Returns the parent of this node.
        virtual Shared parent() const;
    };
}

#endif // Node_h