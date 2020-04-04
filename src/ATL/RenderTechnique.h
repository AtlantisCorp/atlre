//
//  RenderTechnique.h
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#ifndef ATL_RENDERTECHNIQUE_H
#define ATL_RENDERTECHNIQUE_H

#include "Emitter.h"
#include "Renderable.h"
#include "RenderCommand.h"
#include "RenderNode.h"
#include "Camera.h"
#include "Frustum.h"
#include "Resource.h"

namespace Atl
{
    class RenderTechnique;
    ATL_BASE_RESOURCE(RenderTechnique)
    
    //! @brief Listener for RenderTechnique.
    class RenderTechniqueListener : public ListenerBase
    {
    public:
        //! @brief Destructor for RenderTechniqueListener.
        virtual ~RenderTechniqueListener() = default;
        
        //! @brief Launched when the Technique has sorted nodes.
        //! @param emitter The Technique that emitted this event.
        //! @param nodesCount The number of nodes added to the map.
        virtual void onTechniqueDidSortNodes(const RenderTechnique& emitter, std::size_t nodesCount){}
    };
    
    //! @brief Defines a 'Technique' to render and sort a RenderNode Tree.
    class RenderTechnique : 
        public std::enable_shared_from_this < RenderTechnique >,
        public Emitter
    {
    public:
        //! @brief The RenderTechnique's listener.
        typedef RenderTechniqueListener Listener;
        
        //! @brief Default destructor.
        virtual ~RenderTechnique() = default;
        
        //! @brief Renders a node into a command.
        //! @param command The command where to render the node.
        //! @param node The RenderNode we want to render.
        //! @param camera The Camera from where we want to render the node.
        virtual void render(RenderCommand& command, const RenderNode& node, const Camera& camera) const;
        
    protected:
        
        //! @brief Defines a sorted map of nodes, by distance (usually from the camera).
        typedef std::map < Real, RenderableList > NodesMap;
        
        //! @brief Sort a node and its children into the NodesMap.
        //! This function should either add the node to the map, or reject it if it doesn't meet the
        //! requirements.
        //! @param node The node to filter.
        //! @param camera The camera from which we filter the node.
        //! @param frustum The Frustum from which we filter the node. This Frustum is computed at first
        //! so we don't have to recalculate it.
        //! @param nodes The nodes map where we out the filtered node.
        //! @return The number of nodes we have added to the map.
        virtual std::size_t sort(const RenderNode& node, const Camera& camera, const Frustum& frustum, NodesMap& nodes) const = 0;
    };
}

#endif /* ATL_RENDERTECHNIQUE_H */
