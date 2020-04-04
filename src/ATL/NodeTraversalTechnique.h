//
//  NodeTraversalTechnique.h
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#ifndef ATL_NODETRAVERSALTECHNIQUE_H
#define ATL_NODETRAVERSALTECHNIQUE_H

#include "RenderTechnique.h"

namespace Atl
{
    //! @brief A classic Node Traversal technique.
    //! This is the default RenderTechnique used by \ref RenderScene. This technique iterates
    //! through nodes and draw them one by one. Nodes aren't sorted in any way, but can be
    //! culled from the Camera's Frustum when \ref mCullNodes is true.
    class NodeTraversalTechnique : public RenderTechnique
    {
        //! @brief Boolean true if this technique should cull Nodes.
        std::atomic < bool > mCullNodes;
        
    public:
        ATL_SHAREABLE(NodeTraversalTechnique)
        
        //! @brief Constructs a new Technique.
        NodeTraversalTechnique(bool cullNodes = true);
        
        //! @brief Destructor.
        virtual ~NodeTraversalTechnique() = default;
        
    protected:
        
        //! @brief Adds all nodes one by one into nodes.
        virtual std::size_t sort(const RenderNode& node, const Camera& camera, const Frustum& frustum, NodesMap& nodes) const;
    };
}

#endif /* ATL_NODETRAVERSALTECHNIQUE_H */
