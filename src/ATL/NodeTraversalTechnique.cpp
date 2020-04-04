//
//  NodeTraversalTechnique.cpp
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#include "NodeTraversalTechnique.h"

namespace Atl
{
    NodeTraversalTechnique::NodeTraversalTechnique(bool cullNodes)
    : mCullNodes(cullNodes)
    {
        
    }
    
    std::size_t NodeTraversalTechnique::sort(const RenderNode& node, const Camera& camera, const Frustum& frustum, RenderTechnique::NodesMap& nodes) const
    {
        // In this technique we only render node if node is visible && not culled out, or visible
        // && doesn't have any AABB. Then we iterate through its children.
        
        if (node.isVisible())
        {
            if (mCullNodes && node.isCulledFromFrustum(frustum))
                return 0;
            
            const bool isRenderRenderablesFirst = node.renderRenderablesFirst();
            
            RenderNode& notCstNode = const_cast < RenderNode& >(node);
            RenderNodePtr renderNode = std::dynamic_pointer_cast < RenderNode >(notCstNode.shared_from_this());
            
            if (!renderNode)
                throw NullError("NodeTraversalTechnique", "sort", "Node isn't castable to RenderNode.");
            
            if (isRenderRenderablesFirst)
                nodes[0.0].push_back(renderNode);
            
            std::size_t childrenCount = node.childrenCount();
            std::size_t nodesAdded = 1;
            
            for (unsigned i = 0; i < childrenCount; ++i)
            {
                const RenderNode& iNode = dynamic_cast < const RenderNode& >(node.childAt(i));
                nodesAdded += sort(iNode, camera, frustum, nodes);
            }
            
            if (!isRenderRenderablesFirst)
                nodes[0.0].push_back(renderNode);
            
            return nodesAdded;
        }
        
        return 0;
    }
}
