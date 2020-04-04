//
//  TransparentTechnique.cpp
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#include "TransparentTechnique.h"
#include "Material.h"

namespace Atl
{
    std::size_t TransparentTechnique::sort(const RenderNode& node, const Camera& camera, const Frustum& frustum, NodesMap& nodes) const
    {
        try
        {
            const Material& material = node.findRenderable < Material >();
            
            if (!material.isTransparent())
            {
                // If Material is opaque, we can still render children if they are
                // transparent, but our renderable is dead.
                throw RenderNodeNoRenderable("TransparentTechnique", "sort", "Material %s is opaque.",
                                             material.name().data());
            }
            
            return FarthestTechnique::sort(node, camera, frustum, nodes);
        }
        
        catch(RenderNodeNoRenderable const&)
        {
            // If we don't have any Material Renderable, then we don't render
            // this Renderable but we still can render children.
            
            std::size_t childrenCount = node.childrenCount();
            std::size_t nodesAdded = 0;
            
            for (unsigned i = 0; i < childrenCount; ++i)
            {
                const RenderNode& iNode = dynamic_cast < const RenderNode& >(node.childAt(i));
                nodesAdded += sort(iNode, camera, frustum, nodes);
            }
            
            return nodesAdded;
        }
    }
}
