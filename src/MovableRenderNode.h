//
//  MovableRenderNode.h
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#ifndef MovableRenderNode_h
#define MovableRenderNode_h

#include "RenderNode.h"
#include "Transformation.h"

namespace Atl
{
    //! @brief Defines a RenderNode that hold a Transformation.
    class EXPORTED MovableRenderNode : virtual public RenderNode
    {
        //! @brief The Transformation associated to this RenderNode.
        TransformationPtr mTransformation;

    public:
        ATL_SHAREABLE(MovableRenderNode)

        //! @brief Constructs a new MovableRenderNode.
        //! @param parent The node's parent.
        //! @param transformation A Pointer to a Transformation object.
        //! @param maxChildren The maximum number of children for the node.
        //! @param maxRenderables The maximum number of renderables in this node.
        MovableRenderNode(const Node::Shared& parent, 
            const TransformationPtr& transformation, 
            const std::size_t& maxChildren = 0,
            const std::size_t& maxRenderables = 0);

        //! @brief Returns a reference to the transformation.
        Transformation& transformation();

        //! @brief Returns a reference to the transformation.
        const Transformation& transformation() const;

        //! @brief Modifies the Transformation such as this node 'looks' at the given 
        //! target. Please notes it doesn't translate the node.
        MovableRenderNode& lookAt(const rvec3& target, const rvec3& up = rvec3(0.0, 1.0, 0.0));

        //! @brief Returns the position of this node.
        rvec3 position() const;

        //! @brief Changes the Transformation applied to this node.
        void setTransformation(const TransformationPtr& rhs);

        //! @brief Sets the position for this node.
        void setPosition(const rvec3& rhs);

        //! @brief Translates the node.
        void translate(const rvec3& rhs);
    };

    //! @brief Pointer to MovableRenderNode.
    typedef std::shared_ptr < MovableRenderNode > MovableRenderNodePtr;
}

#endif // MovableRenderNode_h
