//
//  RenderTechnique.cpp
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#include "RenderTechnique.h"

namespace Atl
{
    void RenderTechnique::render(RenderCommand& command, const RenderNode& node, const Camera& camera) const
    {
        NodesMap nodes;
        Frustum frustum(camera.matrix());
        
        std::size_t nodesAdded = sort(node, camera, frustum, nodes);
        send(&Listener::onTechniqueDidSortNodes, *this, (std::size_t)nodesAdded);
        
        // Now we have our list of nodes, we can render them one by one by calling only RenderNode::render() on
        // them, as we only want their list of renderables without any child.
        
        std::for_each(nodes.begin(), nodes.end(), [&command](auto& pair)
        {
            auto& list = pair.second;
            
            std::for_each(list.begin(), list.end(), [&command](auto& rhs)
            {
                rhs->render(command);
            });
        });
    }
}
