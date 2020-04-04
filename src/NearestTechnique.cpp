//
//  NearestTechnique.cpp
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#include "NearestTechnique.h"

namespace Atl
{
    std::size_t NearestTechnique::sort(const RenderNode& node, const Camera& camera, const Frustum& frustum, RenderTechnique::NodesMap& nodes) const
    {
        if (!node.isVisible())
            return 0;
        
        if (node.isCulledFromFrustum(frustum))
            return 0;
        
        const bool isRenderRenderablesFirst = node.renderRenderablesFirst();
        rvec3 distance = rvec3(INFINITY, INFINITY, INFINITY);
        RenderNodePtr renderNode = std::dynamic_pointer_cast < RenderNode >(const_cast < RenderNode& >(node).shared_from_this());
        
        if (!renderNode)
            throw NullError("NodeTraversalRenderTechnique", "sort", "Node isn't castable to RenderNode.");
        
        if (node.hasAABB())
        {
            AABB bbox = node.aabb();
            const rvec3 center = bbox.center();
            distance = camera.distance(center);
        }
        
        if (isRenderRenderablesFirst)
            nodes[distance.length()].push_back(renderNode);
        
        std::size_t childrenCount = node.childrenCount();
        std::size_t nodesAdded = 1;
        
        for (unsigned i = 0; i < childrenCount; ++i)
        {
            const RenderNode& iNode = dynamic_cast < const RenderNode& >(node.childAt(i));
            nodesAdded += sort(iNode, camera, frustum, nodes);
        }
        
        if (!isRenderRenderablesFirst)
            nodes[distance.length()].push_back(renderNode);
        
        return nodesAdded;
    }
}
