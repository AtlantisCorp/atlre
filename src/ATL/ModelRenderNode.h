//
//  ModelRenderNode.h
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#ifndef ModelRenderNode_h
#define ModelRenderNode_h

#include "RenderNode.h"
#include "Model.h"

namespace Atl
{
    class ModelRenderNode;

    //! @brief Defines a listener for the ModelRenderNode Class.
    class EXPORTED ModelRenderNodeListener : public RenderNodeListener 
    {
    public:
        //! @brief Destructs the listener.
        virtual ~ModelRenderNodeListener() = default;

        //! @brief Launched when the Model is modified.
        virtual void onModelModified(ModelRenderNode&, const Model&){}
    };

    //! @brief Defines a RenderNode with a Model.
    class EXPORTED ModelRenderNode : virtual public RenderNode 
    {
        //! @brief The Model in this Node.
        ModelPtr mModel;

    public:
        ATL_SHAREABLE(ModelRenderNode)

        //! @brief The listener class.
        typedef ModelRenderNodeListener Listener;

        //! @brief Constructs a new ModelRenderNode.
        //! @param node The node's parent. \see Node::Node.
        //! @param model The model for this node.
        //! @param maxChildren Maximum number of children in this node. Default value
        //! is zero, meaning unlimited.
        //! @param maxRenderables Maximum number of renderables in this node. Default 
        //! value is zero, meaning unlimited.
        ModelRenderNode(const Node::Shared& node,
            const ModelPtr& model,
            const std::size_t& maxChildren = 0,
            const std::size_t& maxRenderables = 1);

        //! @brief Destructs the node.
        virtual ~ModelRenderNode() = default;

        //! @brief Returns the model in this Node.
        virtual ModelPtr model() const;

        //! @brief Changes the model in this Node.
        virtual void setModel(const ModelPtr& model);
    };

    //! @brief Pointer to ModelRenderNode.
    typedef std::shared_ptr < ModelRenderNode > ModelRenderNodePtr;
}

#endif // ModelRenderNode_h
