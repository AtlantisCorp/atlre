//
//  RenderNode.h
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#ifndef RenderNode_h
#define RenderNode_h

#include "Error.h"
#include "Node.h"
#include "Renderable.h"
#include "Emitter.h"
#include "RenderTaskContainer.h"
#include "Frustum.h"
#include "AABB.h"
#include "RenderCommand.h"

namespace Atl
{
    class RenderNode;

    //! @brief Error launched when maximum number of renderables is reached.
    struct EXPORTED RenderNodeMaxRenderables : public Error 
    { using Error::Error; };

    //! @brief Error launched when \ref RenderNode::findRenderable() has no return
    //! possibilities.
    struct EXPORTED RenderNodeNoRenderable : public Error
    { using Error::Error; };

    //! @brief A listener that derives from NodeListener for a RenderNode.
    class EXPORTED RenderNodeListener : public NodeListener
    {
    public:
        //! @brief Destructs the listener.
        virtual ~RenderNodeListener() = default;

        //! @brief Launched when a new Renderable is added to a RenderNode.
        virtual void onRenderNodeDidAddRenderable(RenderNode&, const Renderable&) {}

        //! @brief Launched when a Renderable is removed from the RenderNode.
        virtual void onRenderNodeDidRemoveRenderable(RenderNode&, const Renderable&) {}

        //! @brief Launched when the RenderNode will build a new cache for a renderer.
        virtual void onRenderNodeWillBuild(RenderNode&, Renderer&) {}

        //! @brief Launched when the RenderNode will update a cache for a renderer.
        virtual void onRenderNodeWillUpdate(RenderNode&, Renderer&) {}

        //! @brief Launched when the RenderNode did build a new cache for a renderer.
        virtual void onRenderNodeDidBuild(RenderNode&, Renderer&) {}

        //! @brief Launched when the RenderNode did update a cache for a renderer.
        virtual void onRenderNodeDidUpdate(RenderNode&, Renderer&) {}
    };

    //! @brief Defines a RenderNode.
    //! A RenderNode is a Node which can be rendered. Basically, the RenderNode holds
    //! a RenderTaskContainer and can hold multiple Renderables, like a Material, a Model,
    //! etc. Some derived class are provided to handle those Renderables: MaterialRenderNode
    //! provides a RenderNode with access to a Material, ModelRenderNode provides a Model,
    //! and a RenderNode can render its children MaterialRenderNode and ModelRenderNode.
    //!
    //! A RenderNode renders always all its children after rendering itself. To do this, the
    //! RenderNode adds each child to its RenderTaskContainer, as an ordered render task. However,
    //! if its child sets \ref isUnorderedRender(), then it will place it into the unordered
    //! task list.
    class EXPORTED RenderNode :
    virtual public Node,
    virtual public Renderable
    {
    protected:

        //! The Render Task Container.
        mutable RenderTaskContainerPtr mTasks;

        //! @brief A list of Renderable to render with this Node. All renderables 
        //! added to this node are rendered as ordered renders. If you want to add
        //! unordered render tasks, please use a derived class like GenUnorderedRenderNode
        //! or specific classes like MaterialRenderNode, which is unordered because it
        //! doesn't add any sub commands.
        RenderableList mRenderables;

        //! @brief The maximum number of Renderables in this node. Zero means infinite.
        std::atomic < std::size_t > mMaxRenderables;

        //! @brief True if Renderables are rendered before children, false otherwise.
        std::atomic < bool > mRenderRenderablesFirst;

        //! @brief True if this RenderNode is visible, false otherwise. Default is true.
        std::atomic < bool > mIsVisible;

        //! @brief True if this RenderNode renders everything into its own RenderCommand.
        std::atomic < bool > mOwnRenderCommand;

        //! @brief True if we render children, false otherwise. Default is true.
        std::atomic < bool > mRenderChildren;

        //! @brief The RenderNode AABB, if applicable.
        AABB mAABB;

        //! @brief Boolean true if this RenderNode should cull its children/renderables 
        //! on a Frustum basis, false otherwise. On false, the RenderNode will use the basic
        //! render/build system to render its renderables, but its children will still 
        //! render themselves with the Frustum if their property is true.
        std::atomic < bool > mCullOnFrustum;

        //! @brief Non null if this RenderNode has mOwnRenderCommand true.
        mutable RenderCommandPtr mOwnCommand;

        //! @brief The mutex.
        mutable std::mutex mMutex;

    public:
        ATL_SHAREABLE(RenderNode)

        //! @brief Defines this type shared.
        typedef std::shared_ptr < RenderNode > Shared;

        //! @brief Defines the listener type.
        typedef RenderNodeListener Listener;

        //! @brief Constructs a new RenderNode.
        //! @param parent The node's parent. \see Node::Node.
        //! @param maxChildren Maximum number of children in this node. Default value
        //! is zero, meaning unlimited.
        //! @param maxRenderables Maximum number of renderables in this node. Default 
        //! value is zero, meaning unlimited.
        RenderNode(const Node::Shared& parent, 
            const std::size_t& maxChildren = 0, 
            const std::size_t& maxRenderables = 0);

        //! @brief Destructs the RenderNode.
        virtual ~RenderNode() = default;

        //! @brief Builds the RenderTaskContainer.
        //! The RenderTaskContainer is filled only with the Renderables in this node, 
        //! not the children. For children rendering, please see \ref render(command, frustum).
        virtual std::future < void > build(Renderer&);

        //! @brief Renders all tasks in this RenderNode.
        //! If this RenderNode has been touched, it rebuilds the RenderTaskContainer. The
        //! RenderNode is touched if a node has been added/removed, if a renderable has been
        //! added/removed, or if a child has been touched.
        //! @note The RenderTaskContainer is filled only with the Renderables in this node, 
        //! not the children. For children rendering, please see \ref render(command, frustum).
        virtual std::future < void > render(RenderCommand& cmd) const;
        
        //! @brief Returns the sum of all renderables' size() function for a Renderer.
        virtual std::size_t size(Renderer& rhs) const;

        //! @brief Returns true if this RenderNode can be rendered unordered.
        //! Default implementation returns false.
        virtual bool isUnorderedRender() const;

        //! @brief Adds a new Renderable to this node.
        //! @param rhs The Renderable to add.
        //! If the number of renderables in this node exceed \ref mMaxRenderables, then
        //! it launches an RenderNodeMaxRenderables Error.
        virtual void addRenderable(const RenderablePtr& rhs);

        //! @brief Adds multiple Renderables to this node.
        //! @param rhs The RenderableList to add.
        //! If the number of renderables in this node exceed \ref mMaxRenderables, then
        //! it launches a RenderNodeMaxRenderables Error.
        virtual void addRenderables(const RenderableList& rhs);

        //! @brief Inserts a Renderable at given position.
        //! @param idx The position where to insert the Renderable. If this position is 
        //! invalid, it throws an OutOfRange Error. Please check \ref renderablesCount()
        //! before setting your index.
        //! @param rhs The Renderable to add.
        //! If the number of renderables in this node exceed \ref mMaxRenderables, then
        //! it launches an RenderNodeMaxRenderables Error.
        virtual void insertRenderable(const std::size_t& idx, const RenderablePtr& rhs);

        //! @brief Inserts multiple Renderables at given position.
        //! @param idx The position where to insert the Renderables. If this position is 
        //! invalid, it throws an OutOfRange Error. Please check \ref renderablesCount()
        //! before setting your index.
        //! @param rhs The Renderables to add.
        //! If the number of renderables in this node exceed \ref mMaxRenderables, then
        //! it launches an RenderNodeMaxRenderables Error.
        virtual void insertRenderables(const std::size_t& idx, const RenderableList& rhs);

        //! @brief Removes the Renderable at given index.
        //! @param idx The position where to remove the Renderables. If this position is 
        //! invalid, it throws an OutOfRange Error. Please check \ref renderablesCount()
        //! before setting your index.
        virtual void removeRenderableAt(const std::size_t& idx);

        //! @brief Removes given Renderable.
        //! @param rhs The Renderable to remove. If null, or if not found, this function
        //! returns normally but left the renderable's list unchanged.
        virtual void removeRenderable(const RenderablePtr& rhs);

        //! @brief Removes all Renderables.
        virtual void removeAllRenderables();

        //! @brief Returns the number of Renderables in this node.
        virtual std::size_t renderablesCount() const;

        //! @brief Returns a reference to the Renderable at given index.
        virtual Renderable& renderableAt(const std::size_t& idx);

        //! @brief Returns a reference to the Renderable at given index.
        virtual const Renderable& renderableAt(const std::size_t& idx) const;

        //! @brief Returns the maximum number of Renderables in this Node.
        virtual std::size_t maxRenderables() const;

        //! @brief Changes the maximum number of Renderables in this Node.
        virtual void setMaxRenderables(const std::size_t& num);

        //! @brief Returns \ref mRenderRenderablesFirst.
        virtual bool renderRenderablesFirst() const;

        //! @brief Sets \ref mRenderRenderablesFirst.
        virtual void setRenderRenderablesFirst(bool rhs);

        //! @brief Returns true if this RenderNode has an Axis Aligned Bounding Box (AABB).
        virtual bool hasAABB() const;

        //! @brief Returns \ref mAABB. Weither this member is relevant or not is given 
        //! by \ref hasAABB().
        virtual AABB aabb() const;

        //! @brief Returns \ref mIsVisible.
        virtual bool isVisible() const;

        //! @brief Sets \ref mIsVisible.
        virtual void setVisible(bool rhs);

        //! @brief Renders the RenderNode and its children only if the given \ref Frustrum
        //! doesn't cull the node. 
        //! At the contrary of render(command), this version renders the renderables AND the 
        //! children, if this node is visible, in the frustum (not culled).
        virtual std::future < void > render(RenderCommand& command, const Frustum& frustrum) const;

        //! @brief Sets \ref mOwnRenderCommand.
        virtual void setOwnRenderCommand(bool rhs);

        //! @brief Returns \ref mOwnRenderCommand.
        virtual bool ownRenderCommand() const;

        //! @brief Sets \ref mCullOnFrustum.
        virtual void setCullOnFrustum(bool rhs);

        //! @brief Returns \ref mCullOnFrustum.
        virtual bool cullOnFrustum() const;

        //! @brief Sets \ref mRenderChildren.
        virtual void setRenderChildren(bool rhs);

        //! @brief Returns \ref mRenderChildren.
        virtual bool isRenderChildren() const;

        //! @brief Finds a renderable with the same type as T.
        //! This function uses dynamic_cast to cast the RenderablePtr for each renderable,
        //! untill it finds a renderable suitable.
        template < typename T > T& findRenderable()
        {
            std::lock_guard l(mMutex);
            
            for (RenderablePtr& renderable : mRenderables)
            {
                Renderable* mem = renderable.get();
                if (!mem) continue;
                
                T* casted = dynamic_cast < T* >(mem);
                if (!casted) continue;
                
                return *casted;
            }
            
            throw RenderNodeNoRenderable("RenderNode", "findRenderable", "No Renderable found with type %s.", typeid(T).name());
        }

        //! @brief Finds a renderable with the same type as T.
        //! This function uses dynamic_cast to cast the RenderablePtr for each renderable,
        //! untill it finds a renderable suitable.
        template < typename T > const T& findRenderable() const
        {
            std::lock_guard l(mMutex);
            
            for (const RenderablePtr& renderable : mRenderables)
            {
                const Renderable* mem = renderable.get();
                if (!mem) continue;
                
                const T* casted = dynamic_cast < const T* >(mem);
                if (!casted) continue;
                
                return *casted;
            }
            
            throw RenderNodeNoRenderable("RenderNode", "findRenderable", "No Renderable found with type %s.",
                                         typeid(T).name());
        }

        //! @brief Returns true if the RenderNode is culled from given frustum.
        //! This function calls \ref Frustum::isBoxVisible() if this RenderNode has an
        //! AABB. If not, returns false.
        bool isCulledFromFrustum(const Frustum& frustum) const;
    };

    //! @brief Pointer to a RenderNode.
    typedef std::shared_ptr < RenderNode > RenderNodePtr;

    //! @brief List of pointers to RenderNode.
    typedef std::vector < RenderNodePtr > RenderNodeList;
}

#endif // RenderNode_h
